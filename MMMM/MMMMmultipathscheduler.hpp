// Copyright (c) 2023. ByteDance Inc. All rights reserved.

#pragma once


#include <vector>
#include <set>
#include "basefw/base/log.h"
#include "multipathschedulerI.h"
#include <numeric>


/// min RTT Round Robin multipath scheduler
class MMMMMultiPathScheduler : public MultiPathSchedulerAlgo
{
public:
    MultiPathSchedulerType SchedulerType() override
    {
        return MultiPathSchedulerType::MULTI_PATH_SCHEDULE_MMMM;
    }

    explicit MMMMMultiPathScheduler(const fw::ID& taskid,
            std::map<fw::ID, fw::shared_ptr<SessionStreamController>>& dlsessionmap,
            std::set<DataNumber>& downloadQueue, std::set<int32_t>& lostPiecesQueue)
            : MultiPathSchedulerAlgo(taskid, dlsessionmap, downloadQueue, lostPiecesQueue)
    {
        SPDLOG_DEBUG("taskid :{}", taskid.ToLogStr());
    }

    ~MMMMMultiPathScheduler() override
    {
        SPDLOG_TRACE("");
    }

    int32_t StartMultiPathScheduler(fw::weak_ptr<MultiPathSchedulerHandler> mpsHandler) override
    {
        SPDLOG_DEBUG("");
        m_phandler = std::move(mpsHandler);
        return 0;
    }

    bool StopMultiPathScheduler() override
    {
        SPDLOG_DEBUG("");
        OnResetDownload();
        return true;
    }

    void OnSessionCreate(const fw::ID& sessionid) override
    {
        SPDLOG_DEBUG("session: {}", sessionid.ToLogStr());
        auto&& itor = m_session_needdownloadpieceQ.find(sessionid);
        if (itor != m_session_needdownloadpieceQ.end())
        {// found, clear the sending queue
            SPDLOG_WARN("Session: {} is already created", sessionid.ToLogStr());
            for (auto&& subpiecetask: itor->second)
            {
                m_downloadQueue.emplace(subpiecetask);
            }
        }
        m_session_needdownloadpieceQ[sessionid] = std::set<int32_t>();

    }

    void OnSessionDestory(const fw::ID& sessionid) override
    {
        SPDLOG_DEBUG("session: {}", sessionid.ToLogStr());
        // find the session's queue, clear the subpieces and add the subpieces to main downloading queue
        auto&& itor = m_session_needdownloadpieceQ.find(sessionid);
        if (itor == m_session_needdownloadpieceQ.end())
        {
            SPDLOG_WARN("Session: {} isn't in session queue", sessionid.ToLogStr());
            return;
        }
        for (auto&& subpiecetask: itor->second)
        {
            m_downloadQueue.emplace(subpiecetask);

        }
        m_session_needdownloadpieceQ.erase(itor);
    }

    void OnResetDownload() override
    {
        SPDLOG_DEBUG("");

        if (m_session_needdownloadpieceQ.empty())
        {
            return;
        }
        for (auto& it_sn: m_session_needdownloadpieceQ)
        {
            if (!it_sn.second.empty())
            {
                m_downloadQueue.insert(it_sn.second.begin(), it_sn.second.end());

                it_sn.second.clear();
            }
        }
        m_session_needdownloadpieceQ.clear();

    }

    void DoMultiPathSchedule() override
    {
        if (m_session_needdownloadpieceQ.empty())
        {
            SPDLOG_DEBUG("Empty session map");
            return;
        }
        min_RTT = Duration::FromMicroseconds(5000000);
        for(auto&& sessionItor: m_dlsessionmap)
        {
            auto srtt=sessionItor.second->GetRtt();
            if(srtt<min_RTT) min_RTT=srtt;
            SPDLOG_WARN("min_RTT:{}",min_RTT.ToDebuggingValue());
        }
        cwnd = ceil(BW*(delta_N/min_RTT.ToMicroseconds())*500000/1024);
        SetSessionCwnd();
        // sort session first
        SPDLOG_DEBUG("DoMultiPathSchedule, cwnd: {}, bw: {}", cwnd, BW);
        SortSession(m_sortmmap);
        // send pkt requests on each session based on ascend order;
        FillUpSessionTask();

    }

        uint32_t DoSinglePathSchedule(const fw::ID& sessionid) override
    {
        SPDLOG_DEBUG("session:{}", sessionid.ToLogStr());
        // if key doesn't map to a valid set, []operator should create an empty set for us
        auto& session = m_dlsessionmap[sessionid];
        if (!session)
        {
            SPDLOG_WARN("Unknown session: {}", sessionid.ToLogStr());
            return -1;
        }

        auto uni32DataReqCnt = session->CanRequestPktCnt();
        SPDLOG_DEBUG("Free Wnd : {}", uni32DataReqCnt);
        // try to find how many pieces of data we should fill in sub-task queue;
        if (uni32DataReqCnt == 0)
        {
            SPDLOG_WARN("Free Wnd equals to 0");
            return -1;
        }

        if (m_downloadQueue.size() < uni32DataReqCnt)
        {
            auto handler = m_phandler.lock();
            if (handler)
            {
                handler->OnRequestDownloadPieces(uni32DataReqCnt - m_downloadQueue.size());
            }
            else
            {
                SPDLOG_ERROR("handler = null");
            }
        }

        /// Add task to session task queue
        std::vector<int32_t> vecSubpieceNums;
        // eject uni32DataReqCnt number of subpieces from
        for (auto itr = m_downloadQueue.begin(); itr != m_downloadQueue.end() && uni32DataReqCnt > 0;)
        {
            vecSubpieceNums.push_back(*itr);
            m_downloadQueue.erase(itr++);
            --uni32DataReqCnt;
        }

        m_session_needdownloadpieceQ[sessionid].insert(vecSubpieceNums.begin(), vecSubpieceNums.end());

        ////////////////////////////////////DoSendRequest
        DoSendSessionSubTask(sessionid);
        return 0;
    }

    void OnTimedOut(const fw::ID& sessionid, const std::vector<int32_t>& pns) override
    {
        SPDLOG_DEBUG("session {},lost pieces {}", sessionid.ToLogStr(), pns);
        for (auto& pidx: pns)
        {
            auto&& itor_pair = m_lostPiecesQueue.emplace(pidx);
            if (!itor_pair.second)
            {
                SPDLOG_WARN(" pieceId {} already marked lost", pidx);
            }
        }
    }

    void OnReceiveSubpieceData(const fw::ID& sessionid, SeqNumber seq, DataNumber pno, Timepoint recvtime) override
    {
        //SPDLOG_WARN("session:{}, seq:{}, pno:{}, recvtime:{}",
                //sessionid.ToLogStr(), seq, pno, recvtime.ToDebuggingValue());
        /// rx and tx signal are forwarded directly from transport controller to session controller
        recvtoPno[recvSeq] = pno;
        recvtics[recvSeq] = recvtime.ToDebuggingValue();
        if(recvSeq==0)
        {
            BW=(double)1024/(double)(recvtime.ToDebuggingValue()-sendtics[pno]);
            lastSeq=recvSeq;
            //SPDLOG_WARN("case1:");
        }
        else if(recvtics[recvSeq]-recvtics[recvSeq-1]>=50000||recvSeq-lastSeq<5)//刚开始小，改成receive
        {
            uint64_t timedi=recvtics[recvSeq]-recvtics[recvSeq-1];
            
            double temp1=double(recvtime.ToDebuggingValue()-sendtics[recvtoPno[lastSeq]]);
            double temp2=bBeta*(double)1024*(recvSeq-lastSeq);
            double temp3=temp2/temp1;
            double temp4=(1-bBeta)*BW;
            BW=temp3+temp4;
            //if(timedi>=50000) {lastSeq=recvSeq;}
            //SPDLOG_WARN("case2: recvtic {}, lastrcvtic {}, diff {}, recvSeq {}, lastrecvSeq {}, diff {}",recvtics[recvSeq], recvtics[recvSeq-1], recvSeq, lastSeq, timedi);
        }
        else
        {
            double temp1=double(recvtime.ToDebuggingValue()-sendtics[recvtoPno[lastSeq]]);
            double temp2=Beta*(double)1024*(recvSeq-lastSeq);
            double temp3=temp2/temp1;
            double temp4=(1-Beta)*BW;
            BW=temp3+temp4;
            //BW=(1-Beta)*BW + Beta*(double)1024*(recvSeq-lastSeq)/(double)(recvtime.ToDebuggingValue()-sendtics[recvtoPno[lastSeq]]);
            //SPDLOG_WARN("case3: temp1 {}, temp2 {}, temp3 {}, temp4 {}", temp1, temp2, temp3, temp4);
        }
        SPDLOG_ERROR(", totalBW, ts:{}, totalBW:{}", recvtime.ToDebuggingValue(), BW);
        recvSeq++;
        //SPDLOG_WARN("BW:{}, sendtics:{}, lastsendtics:{}",100*BW, sendtics[pno], sendtics[recvtoPno[lastSeq]]);

        DoSinglePathSchedule(sessionid);
    }

    void SortSession(std::multimap<Duration, fw::shared_ptr<SessionStreamController>>& sortmmap) override
    {
        SPDLOG_TRACE("");
        sortmmap.clear();
        for (auto&& sessionItor: m_dlsessionmap)
        {
            auto score = sessionItor.second->GetRtt();
            sortmmap.emplace(score, sessionItor.second);
        }

    }

    void Ondatasent(std::map<DataNumber, uint64_t> m_sendtic) override
    {
        sendtics=m_sendtic;
    }

    void SetSessionCwnd()//计算每个session的cwnd
    {
        double sessionBW;
        double totalBW=0;
        for (auto&& itor: m_dlsessionmap)
        {
            auto& sessionId = itor.first;
            auto& sessStream = itor.second;
            totalBW = totalBW+sessStream->GetBW();
        }
        Timepoint now_t = Clock::GetClock()->Now();
        SPDLOG_ERROR(", totalcwnd, ts:{}, totalcwnd:{}",now_t.ToDebuggingValue(),cwnd);
        for (auto&& itor: m_dlsessionmap)
        {
            auto& sessionId = itor.first;
            auto& sessStream = itor.second;
            sessionBW = sessStream->GetBW();
            u_int32_t s_cwnd= ceil(cwnd*(sessionBW/totalBW)*((double)200000/(double)sessStream->GetRtt().ToMicroseconds()));
            sessStream->setS_cwnd(s_cwnd);
            //SPDLOG_WARN("sessionID: {}, cwnd: {}, BW: {}", sessionId.ToLogStr(), s_cwnd, sessionBW);
            SPDLOG_ERROR(", sessioncwnd, sessionID:{}, ts:{}, cwnd:{}", sessionId.ToLogStr(), now_t.ToDebuggingValue(), s_cwnd);
        }
    }

private:
    int32_t DoSendSessionSubTask(const fw::ID& sessionid) override
    {
        SPDLOG_TRACE("session id: {}", sessionid.ToLogStr());
        int32_t i32Result = -1;
        auto& setNeedDlSubpiece = m_session_needdownloadpieceQ[sessionid];
        if (setNeedDlSubpiece.empty())
        {
            SPDLOG_TRACE("empty sending queue");
            return i32Result;
        }

        auto& session = m_dlsessionmap[sessionid];
        uint32_t u32CanSendCnt = session->CanRequestPktCnt();
        std::vector<int32_t> vecSubpieces;
        //int i = 0;
        //bool rt = 1;
        for (auto itor = setNeedDlSubpiece.begin();
             itor != setNeedDlSubpiece.end() && vecSubpieces.size() < u32CanSendCnt;)
        {
            vecSubpieces.emplace_back(*itor);
            setNeedDlSubpiece.erase(itor++);
        }
        //     i++;
        //     if(i%8==0)
        //     {
        //         rt = m_dlsessionmap[sessionid]->DoRequestdata(sessionid,vecSubpieces);
        //         if(rt) 
        //         {
        //             vecSubpieces.clear();
        //         }
        //         else 
        //         {
        //             break;
        //         }
        //     }
        // }
        // if(rt)
        // {
        //     rt = m_dlsessionmap[sessionid]->DoRequestdata(sessionid, vecSubpieces);
        // }
        bool rt = m_dlsessionmap[sessionid]->DoRequestdata(sessionid, vecSubpieces);
        if (rt)
        {
            i32Result = 0;
            //succeed
        }
        else
        {
            // fail
            // return sending pieces to main download queue
            SPDLOG_DEBUG("Send failed, Given back");
            //setNeedDlSubpiece.insert(vecSubpieces.begin(),vecSubpieces.end());
            m_downloadQueue.insert(setNeedDlSubpiece.begin(), setNeedDlSubpiece.end());
        }

        return i32Result;
    }

    void FillUpSessionTask()
    {
        // 1. put lost packets back into main download queue
        SPDLOG_TRACE("");
        for (auto&& lostpiece: m_lostPiecesQueue)
        {
            auto&& itor_pair = m_downloadQueue.emplace(lostpiece);
            if (itor_pair.second)
            {
                SPDLOG_DEBUG("lost piece {} inserts successfully", lostpiece);
            }
            else
            {
                SPDLOG_DEBUG("lost piece {} already in task queue", lostpiece);
            }
        }
        m_lostPiecesQueue.clear();

        // 2. go through every session,find how many pieces we can request at one time

        std::map<basefw::ID, uint32_t> toSendinEachSession;
        uint32_t maxWeight = 0;
        for (auto&& itor: m_dlsessionmap)
        {
            auto& sessionId = itor.first;
            auto& sessStream = itor.second;
            auto sessCanSendCnt = sessStream->CanRequestPktCnt();
            if(sessCanSendCnt>maxWeight)//求最大权重
            {
                maxWeight=sessCanSendCnt;
            }
            toSendinEachSession.emplace(sessionId, sessCanSendCnt);
            if (sessCanSendCnt != 0)
            {
                SPDLOG_DEBUG("session {} has {} free wnd", sessionId.ToLogStr(), sessCanSendCnt);
            }
        }
        uint32_t totalSubpieceCnt = std::accumulate(toSendinEachSession.begin(), toSendinEachSession.end(),
                0, [](size_t total,
                        std::pair<const basefw::ID, uint32_t>& session_task_itor)
                {
                    return total + session_task_itor.second;
                });

        // 3. try to request enough piece cnt from up layer, if necessary

        if (m_downloadQueue.size() < totalSubpieceCnt)
        {
            auto handler = m_phandler.lock();
            if (handler)
            {
                handler->OnRequestDownloadPieces(totalSubpieceCnt - m_downloadQueue.size());
            }
            else
            {
                SPDLOG_ERROR("handler = null");
            }
        }

        SPDLOG_TRACE(" download queue size: {}, need pieces cnt: {}", m_downloadQueue.size(), totalSubpieceCnt);

        // 4. fill up each session Queue, based on min RTT first order, and send
        std::vector<DataNumber> vecToSendPieceNum;
        auto&& itrmQueue = m_downloadQueue.begin();
        for(int i = 0;i<maxWeight;i++)
        {
            for(auto&& IDitor:toSendinEachSession)
            {
                auto& sessionId = IDitor.first;
                if(itrmQueue != m_downloadQueue.end()&&toSendinEachSession[sessionId]>0)
                {
                    m_session_needdownloadpieceQ[sessionId].insert(*itrmQueue);
                    m_downloadQueue.erase(itrmQueue++);
                    toSendinEachSession[sessionId]=toSendinEachSession[sessionId]-1;
                }
            
            }

        }
                // then send in each session
        for (auto&& it_sn = m_session_needdownloadpieceQ.begin();
             it_sn != m_session_needdownloadpieceQ.end(); ++it_sn)
        {
            auto& sessId = it_sn->first;
            auto& sessQueue = it_sn->second;
            SPDLOG_TRACE("session Id:{}, session queue:{}", sessId.ToLogStr(), sessQueue);
            DoSendSessionSubTask(it_sn->first);
        }


    }// end of FillUpSessionTask

    /// It's multipath scheduler's duty to maintain session_needdownloadsubpiece, and m_sortmmap
    double BW = 0.1;
    int delta = 0;
    double delta_N = 200000;
    uint32_t cwnd = 5;
    uint32_t recvSeq = 0;
    uint32_t lastSeq = 0;
    const float Beta = 0.25f;
    const float bBeta = 0.1f;
    Duration min_RTT = Duration::FromMicroseconds(300000);
    std::map<fw::ID, std::set<DataNumber>> m_session_needdownloadpieceQ;// session task queues
    std::multimap<Duration, fw::shared_ptr<SessionStreamController>> m_sortmmap;
    fw::weak_ptr<MultiPathSchedulerHandler> m_phandler;
    std::map<DataNumber, uint64_t> sendtics;
    std::map<DataNumber, uint64_t> recvtics;
    std::map<uint32_t, DataNumber> recvtoPno;

};

