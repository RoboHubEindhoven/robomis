/**
 * @file /include/robomis/robomisnode.hpp
 *
 * @brief Communications central!
 *
 * @date March 2019
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef robomisNODE_HPP
#define robomisNODE_HPP
/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include "std_msgs/String.h"
#include <string>
#include <QThread>
#include <QStringListModel>
#include <move_base_msgs/MoveBaseActionFeedback.h>
#include <nav_msgs/Odometry.h>


#include "geometry_msgs/PoseStamped.h"
//#include "geometry_msgs/Twist.h"
#include <tf/transform_listener.h>
#include <tf/transform_datatypes.h>


#include "robomis/Waypoint.h"
#include "robomis/WaypointData.h"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace ziros {

/*****************************************************************************
** Class
*****************************************************************************/
//typedef void(MainWindow::*BarkFunction)(void);
typedef void (*FeedbackFunctionPtr)(robomis::Waypoint data);
typedef void (*MarkerFunctionPtr)(std::string data);

class robomisNode : public QThread {
    Q_OBJECT
public:
        robomisNode(int argc, char** argv );
        virtual ~robomisNode();
        bool init();
        bool init(const std::string &master_url, const std::string &host_url);
        void run();

        /*********************
        ** Logging
        **********************/
        enum LogLevel {
                 Debug,
                 Info,
                 Warn,
                 Error,
                 Fatal
         };

        QStringListModel* loggingModel() { return &logging_model; }
        void log( const LogLevel &level, const std::string &msg);
        void attachMarkerCallback(MarkerFunctionPtr fPtr) { markerFunctPtr = fPtr; }
        void attachFeedbackCallback(FeedbackFunctionPtr fPtr) { feedbackFunctPtr = fPtr; }

        void addWaypoint(int instance_id, std::string name, bool scan, double pos_x, double pos_y, double pos_z, double orient_x, double orient_y, double orient_z, double orient_w);
        void updateWaypoint(int index, int instance_id, std::string name, bool scan, double pos_x, double pos_y, double pos_z, double orient_x, double orient_y, double orient_z, double orient_w);
        int getLocationIdentifierType(std::string location);
        int getNextLocationInstanceId(std::string location);

        void setWaypointData(robomis::WaypointData rob_msg) { this->rob_msg = rob_msg; }
        robomis::WaypointData *getWaypointData(void) { return &rob_msg; }
        robomis::Waypoint *getWaypointFromFeedback(void) { return &way_msg; }
        int getCurrentWaypointMarker();
        void setFrame(std::string frame) { rob_msg.frame = frame; }
        void publishWaypoints(void);
        geometry_msgs::Pose getRobotPosition();
        void reset();
        void newPosereceived();
Q_SIGNALS:
        void loggingUpdated();
        void newPoseSignal(geometry_msgs::Pose);
        void rosShutdown();

private:
        void waypointMarkerCallback(const std_msgs::String::ConstPtr& msg);
//        void feedbackCallback(const nav_msgs::Odometry::ConstPtr& msg);
        void feedbackCallback(const move_base_msgs::MoveBaseActionFeedback::ConstPtr& msg);
        int getNrOfLocationWaypoints(std::string location);
        std::string createWaypointDescription(std::string location, int instance_id);
        tf::StampedTransform getTransform();
        int init_argc;
        char** init_argv;
        ros::Publisher waypoint_pub;
        ros::Subscriber waypoint_marker_sub;
        ros::Subscriber feedback_sub;
        QStringListModel logging_model;
        robomis::WaypointData rob_msg;
        robomis::Waypoint way_msg;
        std_msgs::String waypoint_marker;
        FeedbackFunctionPtr feedbackFunctPtr;
        MarkerFunctionPtr markerFunctPtr;
        tf::TransformListener * listener;
         tf::StampedTransform transform;

       int shelf_nr, conveyor_nr, workstation_nr, waypoint_nr, pre_plaform_nr;

};

} /* namespace ziros */

#endif // robomisNODE_HPP
