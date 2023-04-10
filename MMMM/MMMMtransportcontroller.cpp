// Copyright (c) 2023. ByteDance Inc. All rights reserved.

#include "MMMMtransportcontroller.hpp"

MMMMTransportCtlConfig::MMMMTransportCtlConfig() : TransPortControllerConfig()
{
}

std::string MMMMTransportCtlConfig::DebugInfo()
{
    std::stringstream ss;
    ss
        << "{"
        << "minWnd:" << minWnd << " maxWnd:" << maxWnd << " slowStartThreshold:" << slowStartThreshold
        << " }";
    return ss.str();
}

MMMMTransportCtl::MMMMTransportCtl(std::shared_ptr<TransPortControllerConfig> ctlConfig)
    : MPDTransportController(ctlConfig)
{
    // multipathscheduler = std::make_shared(RRMultiPathScheduler());
    m_transCtlConfig = std::dynamic_pointer_cast<MMMMTransportCtlConfig>(ctlConfig);
    renoccConfig.minCwnd = m_transCtlConfig->minWnd;
    renoccConfig.maxCwnd = m_transCtlConfig->maxWnd;
    renoccConfig.ssThresh = m_transCtlConfig->slowStartThreshold;
    SPDLOG_DEBUG("config:{}", m_transCtlConfig->DebugInfo());
}

MMMMTransportCtl::~MMMMTransportCtl()
{
    SPDLOG_DEBUG("");
    if (isRunning)
    {
        SPDLOG_ERROR("Destruct before stop");
    }
}

/** @brief The Controller is started
 *  @param tansDlTkInfo  download task info
 *  @param transCtlHandler application layer callback
 **/
bool MMMMTransportCtl::StartTransportController(TransportDownloadTaskInfo tansDlTkInfo,
                                                std::weak_ptr<MPDTransCtlHandler> transCtlHandler)
{
    if (isRunning)
    {
        // warn: already started
        SPDLOG_WARN("Already running");
        return true;
    }
    else
    {
        isRunning = true;
    }
    SPDLOG_DEBUG("taskid: {}", tansDlTkInfo.m_rid.ToLogStr());

    m_transctlHandler = transCtlHandler;

    m_multipathscheduler.reset(
        new MMMMMultiPathScheduler(m_tansDlTkInfo.m_rid, m_sessStreamCtlMap, m_downloadPieces, m_lostPiecesl));
    m_multipathscheduler->StartMultiPathScheduler(shared_from_this());
    return true;
}

/** @brief The controller should be fully stopped and prepare to be destructed.
 * */
void MMMMTransportCtl::StopTransportController()
{
    SPDLOG_DEBUG("Stop Transport Controller isRunning = {}", isRunning);
    if (!isRunning)
    {
        return;
    }
    isRunning = false;
    // todo:
    if (m_multipathscheduler)
    {
        m_multipathscheduler->StopMultiPathScheduler();
    }

    if (!m_sessStreamCtlMap.empty())
    {
        for (auto &&id_sess : m_sessStreamCtlMap)
        {
            if (id_sess.second)
            {
                if (id_sess.second)
                {
                    id_sess.second->StopSessionStreamCtl();
                    id_sess.second.reset();
                }
            }
        }
    }
}

/**
 * @brief Session is ready to send data request
 * @param sessionid
 */
void MMMMTransportCtl::OnSessionCreate(const fw::ID &sessionid)
{
    SPDLOG_TRACE("session = {}", sessionid.ToLogStr());
    if (!isRunning)
    {
        SPDLOG_WARN(" Start session before Start transport Module");
    }
    auto &&sessionItor = m_sessStreamCtlMap.find(sessionid);
    if (sessionItor == m_sessStreamCtlMap.end())
    {
        m_sessStreamCtlMap[sessionid] = std::make_shared<SessionStreamController>();
        m_sessStreamCtlMap[sessionid]->StartSessionStreamCtl(sessionid, renoccConfig, shared_from_this());
    }
    else
    {
        // session recreate again
        SPDLOG_WARN("session:{} already created", sessionid.ToLogStr());
    }
    // forward the message
    if (m_multipathscheduler)
    {
        m_multipathscheduler->OnSessionCreate(sessionid);
    }
    else
    {
        SPDLOG_ERROR("multipathscheduler = null");
    }
}

/**
 * @brief Session is no longer available
 * @param sessionid
 */
void MMMMTransportCtl::OnSessionDestory(const fw::ID &sessionid)
{
    SPDLOG_TRACE("session = {}", sessionid.ToLogStr());
    if (!isRunning)
    {
        SPDLOG_WARN(" Stop session before Start transport Module");
    }
    // forward the message First
    if (m_multipathscheduler)
    {
        m_multipathscheduler->OnSessionDestory(sessionid);
    }
    else
    {
        SPDLOG_ERROR("multipathscheduler = null");
    }
    // do clean jobs
    auto &&sessionItor = m_sessStreamCtlMap.find(sessionid);
    if (sessionItor == m_sessStreamCtlMap.end())
    {
        // warn: try to destroy a session we don't know
    }
    else
    {
        m_sessStreamCtlMap[sessionid]->StopSessionStreamCtl();
        m_sessStreamCtlMap[sessionid].reset();
    }
}

/**
 * @brief The application layer is adding tasks to transport layer.
 * @param datapiecesVec the piece numbers to be added
 */
void MMMMTransportCtl::OnPieceTaskAdding(std::vector<int32_t> &datapiecesVec)
{
    SPDLOG_DEBUG("");
    for (auto &&dataPiece : datapiecesVec)
    {
        auto rt = m_downloadPieces.emplace(dataPiece);
        if (!rt.second)
        {
            // warning: already has
        }
    }
    // Do multipath schedule after new tasks added
    m_multipathscheduler->DoMultiPathSchedule();
}

/**@brief the download task is started now.
 * */
void MMMMTransportCtl::OnDownloadTaskStart()
{
    SPDLOG_DEBUG("");
    if (isRunning)
    {
        if (m_multipathscheduler)
        {
            m_multipathscheduler->StartMultiPathScheduler(shared_from_this());
        }
    }
}

/**@brief the download task is stopped and this object may be destroyed at any time.
 * */
void MMMMTransportCtl::OnDownloadTaskStop()
{
    SPDLOG_DEBUG("");
    StopTransportController();
}

/**@brief the download task has been reset, the task will be stopped soon.
 * */
void MMMMTransportCtl::OnDownloadTaskReset()
{
    SPDLOG_DEBUG("");
    StopTransportController();
}

/** @brief Some data pieces have been received on session with sessionid
 * @param sessionid  the session
 * @param datapiece data pieces number, each packet carry exact one 1KB data piece
 * */
void MMMMTransportCtl::OnDataPiecesReceived(const fw::ID &sessionid, uint32_t seq, int32_t datapiece, uint64_t tic_us)
{
    SPDLOG_TRACE("session = {}, seq ={},datapiece = {},tic_us = {}", sessionid.ToLogStr(), seq, datapiece, tic_us);
    Timepoint recvtic = Clock::GetClock()->CreateTimeFromMicroseconds(tic_us);
    // call session control firstly to change cwnd first
    auto &&sessionItor = m_sessStreamCtlMap.find(sessionid);
    if (sessionItor != m_sessStreamCtlMap.end())
    {
        sessionItor->second->OnDataPktReceived(seq, datapiece, recvtic);
    }
    else
    {
        // warn: received on an unknown session
        SPDLOG_WARN("received on an unknown session {}", sessionid.ToLogStr());
    }
    // inform multipath scheduler
    m_multipathscheduler->OnReceiveSubpieceData(sessionid, seq, datapiece, recvtic);
}

/**
 * @brief Signal that when a data request packet has been sent successfully
 * @note When we say SENT, it means the packet has been given to ASIO to sent. The packet itself may still inside OS
 * kernel space or hardware buffer.
 * @param sessionid remote upside session id
 * @param datapiecesvec the data piece number, each packet may carry 1 piece or 8 pieces
 * @param senttime_us the sent timepoint in us
 */
void MMMMTransportCtl::OnDataSent(const fw::ID &sessionid, const std::vector<int32_t> &datapiecesvec,
                                  const std::vector<uint32_t> &seqvec,
                                  uint64_t senttime_us)
{
    SPDLOG_DEBUG("sessionid = {}, datapieces = {}, seq = {}, senttic = {}",
                 sessionid.ToLogStr(),
                 datapiecesvec,
                 seqvec, senttime_us);
    //Timepoint sendtic = Clock::GetClock()->CreateTimeFromMicroseconds(senttime_us);
    for (auto &datapiece : datapiecesvec)
    {

        m_sendtic[datapiece] = senttime_us;
    }
    m_multipathscheduler->Ondatasent(m_sendtic);
    auto &&sessStreamItor = m_sessStreamCtlMap.find(sessionid);
    if (sessStreamItor != m_sessStreamCtlMap.end())
    {
        sessStreamItor->second->OnDataRequestPktSent(
            seqvec,
            datapiecesvec, Clock::GetClock()->CreateTimeFromMicroseconds(senttime_us));
    }
    else
    {
        SPDLOG_WARN("sessionid = {} can't found in session map ", sessionid.ToLogStr());
    }
}

/**@brief Check timeout events periodically, the user defined timeout check operation may be called in here.
 * */
void MMMMTransportCtl::OnLossDetectionAlarm()
{
    SPDLOG_TRACE("MMMMTransportCtl::OnLossDetectionAlarm()");
    // Step 1: Check loss in each session
    for (auto &&sessStreamItor : m_sessStreamCtlMap)
    {
        sessStreamItor.second->OnLossDetectionAlarm();
    }
    SPDLOG_ERROR(", OnLossDetection, TS:{}, lostsize:{}", Clock::GetClock()->Now().ToDebuggingValue(), m_lostPiecesl.size());
    // Step 2: Forward message to Multipath Scheduler
    m_multipathscheduler->DoMultiPathSchedule();
}

// session stream handler

void MMMMTransportCtl::OnPiecePktTimeout(const basefw::ID &peerid, const std::vector<int32_t> &spns)
{
    if (!isRunning)
    {
        return;
    }

    m_multipathscheduler->OnTimedOut(peerid, spns);
}

bool MMMMTransportCtl::DoSendDataRequest(const basefw::ID &peerid, const std::vector<int32_t> &spns)
{
    SPDLOG_TRACE("peerid = {}, spns= {}", peerid.ToLogStr(), spns);
    if (!isRunning)
    {
        SPDLOG_TRACE("isRunning = false");
        return false;
    }
    else
    {
    }

    auto handler = m_transctlHandler.lock();
    if (handler)
    {
        return handler->DoSendDataRequest(peerid, spns);
    }
    else
    {
        SPDLOG_WARN("handler == null");
        return false;
    }
}

// Multipath scheduler handlers

bool MMMMTransportCtl::OnGetCurrPlayPos(uint64_t &currplaypos)
{
    // curr play pos in Byte
    return false;
}

bool MMMMTransportCtl::OnGetCurrCachePos(uint64_t &currcachepos)
{
    return false;
}

bool MMMMTransportCtl::OnGetByteRate(uint32_t &playbyterate)
{
    // bytes per second
    return false;
}

void MMMMTransportCtl::OnRequestDownloadPieces(uint32_t maxpiececnt)
{
    SPDLOG_DEBUG("maxpiececnt = {}", maxpiececnt);
    if (!isRunning)
    {
        return;
    }
    // ask for more task pieces
    auto handler = m_transctlHandler.lock();
    if (handler)
    {
        handler->DoRequestDatapiecesTask(maxpiececnt);
    }
    else
    {
        SPDLOG_WARN("Handler = null");
    }
}

std::shared_ptr<MPDTransportController>
MMMMTransportCtlFactory::MakeTransportController(std::shared_ptr<TransPortControllerConfig> ctlConfig)
{
    SPDLOG_DEBUG("");
    std::shared_ptr<MPDTransportController> transportControllerImpl(new MMMMTransportCtl(ctlConfig));
    return transportControllerImpl;
}
