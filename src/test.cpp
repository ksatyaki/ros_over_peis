#include <geometry_msgs/PoseStamped.h>
#include <iostream>
#include <ros_over_peis/publisher.h>

int main(int argn, char* args[])
{
	peiskmt_initialize(&argn, args);

	geometry_msgs::PoseStamped p;

	p.header.frame_id = "bull";
	p.pose.position.x = 0.1;
	p.pose.position.y = 1.2;
	p.pose.position.z = 2.3;

	ros_over_peis::Publisher<geometry_msgs::PoseStamped> first_test("k.k");

	first_test.publish(p);

	while(1) {
		p.pose.position.x++;
		sleep(1); first_test.publish(p);}

}
