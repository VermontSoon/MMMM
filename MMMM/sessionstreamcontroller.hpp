// Copyright (c) 2023. ByteDance Inc. All rights reserved.

#pragma once

#include <deque>
#include <memory>
#include "congestioncontrol.hpp"
#include "basefw/base/log.h"
#include "packettype.h"

class SessionStreamCtlHandler
{
public:
    virtual void OnPiecePktTimeout(const basefw::ID& peerid, const std::vector<int32_t>& spns) = 0;

    virtual bool DoSendDataRequest(const basefw::ID& peerid, const std::vector<int32_t>& spns) = 0;
};

/// PacketSender is a simple traffic control module, in TCP or Quic, it is called Pacer.
/// Decide if we can send pkt at this time
class PacketSender
{
public:
    bool CanSend(uint32_t cwnd, uint32_t downloadingPktCnt)
    {

        auto rt = false;
        if (cwnd > downloadingPktCnt)
        {
            rt = true;
        }
        else
        {
            rt = false;
        }
        SPDLOG_TRACE("cwnd:{},downloadingPktCnt:{},rt: {}", cwnd, downloadingPktCnt, rt);
        return rt;
    }

    uint32_t MaySendPktCnt(uint32_t cwnd, uint32_t downloadingPktCnt)
    {
        SPDLOG_TRACE("cwnd:{},downloadingPktCnt:{}", cwnd, downloadingPktCnt);
        if (cwnd >= downloadingPktCnt)
        {
            return std::min(cwnd - downloadingPktCnt, 8U);
        }
        else
        {
            return 0U;
        }
    }

};

/// SessionStreamController is the single session delegate inside transport module.
/// This single session contains three part, congestion control module, loss detection module, traffic control module.
/// It may be used to send data request in its session and receive the notice when packets has been sent
class SessionStreamController
{
public:

    SessionStreamController()
    {
        SPDLOG_TRACE("");
    }

    ~SessionStreamController()
    {
        SPDLOG_TRACE("");
        StopSessionStreamCtl();
    }

    void StartSessionStreamCtl(const basefw::ID& sessionId, RenoCongestionCtlConfig& ccConfig,
            std::weak_ptr<SessionStreamCtlHandler> ssStreamHandler)
    {
        SPDLOG_TRACE("");
        if (isRunning)
        {
            SPDLOG_WARN("isRunning = true");
            return;
        }
        isRunning = true;
        m_sessionId = sessionId;
        m_ssStreamHandler = ssStreamHandler;
        // cc
        m_ccConfig = ccConfig;
        //m_congestionCtl.reset(new RenoCongestionContrl(m_ccConfig));

        // send control
        m_sendCtl.reset(new PacketSender());

        //loss detection
        m_lossDetect.reset(new DefaultLossDetectionAlgo());

        // set initial smothed rtt
        m_rttstats.set_initial_rtt(Duration::FromMilliseconds(200));

    }

    void StopSessionStreamCtl()
    {
        if (isRunning)
        {
            isRunning = false;
        }
        else
        {
            SPDLOG_WARN("isRunning = false");
        }
    }

    basefw::ID GetSessionId()
    {
        if (isRunning)
        {
            return m_sessionId;
        }
        else
        {
            return {};
        }
    }

    bool CanSend()
    {
        SPDLOG_TRACE("");
        if (!isRunning)
        {
            return false;
        }

        //return m_sendCtl->CanSend(m_congestionCtl->GetCWND(), GetInFlightPktNum());
        return m_sendCtl->CanSend(s_cwnd,GetInFlightPktNum());
    }

    uint32_t CanRequestPktCnt()
    {
        SPDLOG_TRACE("");
        if (!isRunning)
        {
            return false;
        }
        //return m_sendCtl->MaySendPktCnt(m_congestionCtl->GetCWND(), GetInFlightPktNum());
        return m_sendCtl->MaySendPktCnt(s_cwnd,GetInFlightPktNum());
    };

    /// send ONE datarequest Pkt, requestting for the data pieces whose id are in spns
    bool DoRequestdata(const basefw::ID& peerid, const std::vector<int32_t>& spns)
    {
        SPDLOG_TRACE("peerid = {}, spns = {}", peerid.ToLogStr(), spns);
        if (!isRunning)
        {
            return false;
        }
        if (!CanSend())
        {
            SPDLOG_WARN("CanSend = false");
            return false;
        }

        if (spns.size() > CanRequestPktCnt())
        {
            SPDLOG_WARN("The number of request data pieces {} exceeds the freewnd {}", spns.size(), CanRequestPktCnt());
            return false;
        }
        auto handler = m_ssStreamHandler.lock();
        if (handler)
        {
            return handler->DoSendDataRequest(peerid, spns);
        }
        else
        {
            SPDLOG_WARN("SessionStreamHandler is null");
            return false;
        }

    }

    void OnDataRequestPktSent(const std::vector<SeqNumber>& seqs,
            const std::vector<DataNumber>& dataids, Timepoint sendtic)
    {
        SPDLOG_TRACE("seq = {}, dataid = {}, sendtic = {}",
                seqs,
                dataids, sendtic.ToDebuggingValue());
        if (!isRunning)
        {
            return;
        }
        auto seqidx = 0;
        for (auto datano: dataids)
        {
            DataPacket p;
            p.seq = seqs[seqidx];
            p.pieceId = datano;
            // add to downloading queue
            m_inflightpktmap.AddSentPacket(p, sendtic);

            // inform cc algo that a packet is sent
            InflightPacket sentpkt;
            sentpkt.seq = seqs[seqidx];
            sentpkt.pieceId = datano;
            sentpkt.sendtic = sendtic;
            //m_congestionCtl->OnDataSent(sentpkt);
            seqidx++;
            sessionSendtic[datano]= sendtic.ToDebuggingValue();
            
        }
        SPDLOG_ERROR(", sessionInflight, sessionID:{}, ts:{}, inflightpkgNum:{}", GetSessionId().ToLogStr(), sendtic.ToDebuggingValue(), GetInFlightPktNum());
    }

    void OnDataPktReceived(uint32_t seq, int32_t datapiece, Timepoint recvtic)
    {
        if (!isRunning)
        {
            return;
        }
        // find the sending record
        auto rtpair = m_inflightpktmap.PktIsInFlight(seq, datapiece);
        auto inFlight = rtpair.first;
        auto inflightPkt = rtpair.second;
        if (inFlight)
        {
            sessionRecvtoPno[recvSeq] = datapiece;
            sessionRecvtics[recvSeq] = recvtic.ToDebuggingValue();

            if(recvSeq==0)
            {
                s_BW=(double)1024/(double)(recvtic.ToDebuggingValue()-sessionSendtic[datapiece]);
                lastSeq=recvSeq;
                //SPDLOG_WARN("case1:");
            }
            else if(sessionRecvtics[recvSeq]-sessionRecvtics[recvSeq-1]>=50000||recvSeq-lastSeq<5)//刚开始小，改成receive
            {
                uint64_t timedi=sessionRecvtics[recvSeq]-sessionRecvtics[recvSeq-1];
                
                double temp1=double(recvtic.ToDebuggingValue()-sessionSendtic[sessionRecvtoPno[lastSeq]]);
                double temp2=bBeta*(double)1024*(recvSeq-lastSeq);
                double temp3=temp2/temp1;
                double temp4=(1-bBeta)*s_BW;
                s_BW=temp3+temp4;
                //if(timedi>=50000) {lastSeq=recvSeq;}
                //SPDLOG_WARN("case2: recvSeq {}, lastrecvSeq {}, gap{}",recvSeq, lastSeq, timedi);
            }
            else
            {
                double temp1=double(recvtic.ToDebuggingValue()-sessionSendtic[sessionRecvtoPno[lastSeq]]);
                double temp2=Beta*(double)1024*(recvSeq-lastSeq);
                double temp3=temp2/temp1;
                double temp4=(1-Beta)*s_BW;
                s_BW=temp3+temp4;
                //BW=(1-Beta)*BW + Beta*(double)1024*(recvSeq-lastSeq)/(double)(recvtime.ToDebuggingValue()-sendtics[recvtoPno[lastSeq]]);
                //SPDLOG_WARN("case3: temp1 {}, temp2 {}, temp3 {}, temp4 {}", temp1, temp2, temp3, temp4);
            }
            SPDLOG_ERROR(", sessionBW, sessionId:{}, ts:{}, BW:{}", m_sessionId.ToLogStr(), recvtic.ToDebuggingValue(), s_BW);
            recvSeq++;
            //SPDLOG_WARN("sessionId:{}, S_BW:{}",m_sessionId, 100*s_BW);

        

            auto oldsrtt = m_rttstats.smoothed_rtt();
            // we don't have ack_delay in this simple implementation.
            auto pkt_rtt = recvtic - inflightPkt.sendtic;
            m_rttstats.UpdateRtt(pkt_rtt, Duration::Zero(), Clock::GetClock()->Now());
            auto newsrtt = m_rttstats.smoothed_rtt();

            //auto oldcwnd = m_congestionCtl->GetCWND();

            AckEvent ackEvent;
            ackEvent.valid = true;
            ackEvent.ackPacket.seq = seq;
            ackEvent.ackPacket.pieceId = datapiece;
            ackEvent.sendtic = inflightPkt.sendtic;
            LossEvent lossEvent; // if we detect loss when ACK event, we may do loss check here.
            //m_congestionCtl->OnDataAckOrLoss(ackEvent, lossEvent, m_rttstats);

            //auto newcwnd = m_congestionCtl->GetCWND();
            // mark as received
            m_inflightpktmap.OnPacktReceived(inflightPkt, recvtic);
        }
        else
        {
            SPDLOG_WARN(" Recv an pkt with unknown seq:{}", seq);
        }

    }

    void OnLossDetectionAlarm()
    {
        DoAlarmTimeoutDetection();
    }

    void InformLossUp(LossEvent& loss)
    {
        if (!isRunning)
        {
            return;
        }
        auto handler = m_ssStreamHandler.lock();
        if (handler)
        {
            std::vector<int32_t> lossedPieces;
            for (auto&& pkt: loss.lossPackets)
            {
                lossedPieces.emplace_back(pkt.pieceId);
            }
            handler->OnPiecePktTimeout(m_sessionId, lossedPieces);
        }
    }

    void DoAlarmTimeoutDetection()
    {
        if (!isRunning)
        {
            return;
        }
        ///check timeout
        Timepoint now_t = Clock::GetClock()->Now();
        AckEvent ack;
        LossEvent loss;
        haslost = 1;
        m_lossDetect->DetectLoss(m_inflightpktmap, now_t, ack, -1, loss, m_rttstats);
        if (loss.valid)
        {
            for (auto&& pkt: loss.lossPackets)
            {
                auto pkt_rtt = m_rttstats.smoothed_rtt();
                m_rttstats.UpdateRtt(pkt_rtt,Duration::Zero(),Clock::GetClock()->Now());//丢包则把rtt设为2.25倍
                m_inflightpktmap.RemoveFromInFlight(pkt);
                s_BW = 0.9*s_BW;
            }
            //m_congestionCtl->OnDataAckOrLoss(ack, loss, m_rttstats);
            InformLossUp(loss);
        }
    }

    Duration GetRtt()
    {
        Duration rtt{ Duration::FromMicroseconds(200000) };
        if (isRunning)
        {
            rtt = m_rttstats.SmoothedOrInitialRtt();
        }
        SPDLOG_DEBUG("rtt = {}", rtt.ToDebuggingValue());
        return rtt;
    }

    double GetBW()
    {
        double bw=0;
        if (isRunning)
        {
            bw = s_BW;
        }
        return bw;
    }

    void setS_cwnd(uint32_t cwnd)
    {
        if(haslost)
        {
            s_cwnd = ceil(0.5*cwnd + 0.5);
            haslost = 0;
        }
        else
        {
            s_cwnd = cwnd;
        }
    }

    uint32_t GetInFlightPktNum()
    {
        return m_inflightpktmap.InFlightPktNum();
    }


    basefw::ID GetSessionID()
    {
        return m_sessionId;
    }

private:
    bool isRunning{ false };
    bool haslost{ false };

    basefw::ID m_sessionId;/** The remote peer id defines the session id*/
    basefw::ID m_taskid;/**The file id downloading*/
    RenoCongestionCtlConfig m_ccConfig;
    //std::unique_ptr<CongestionCtlAlgo> m_congestionCtl;
    std::unique_ptr<LossDetectionAlgo> m_lossDetect;
    std::weak_ptr<SessionStreamCtlHandler> m_ssStreamHandler;
    InFlightPacketMap m_inflightpktmap;
    uint32_t s_cwnd = 5;
    double s_BW = 0.01;
    uint32_t recvSeq = 0;
    uint32_t lastSeq = 0;
    const float Beta = 0.25f;
    const float bBeta = 0.1f;
    std::map<DataNumber, uint64_t> sessionSendtic;
    std::map<DataNumber, uint64_t> sessionRecvtics;
    std::map<uint32_t, DataNumber> sessionRecvtoPno;
    std::unique_ptr<PacketSender> m_sendCtl;
    RttStats m_rttstats;
};

