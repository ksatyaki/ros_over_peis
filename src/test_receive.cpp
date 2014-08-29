#include <ros_over_peis/subscriber.h>
#include <geometry_msgs/PoseStamped.h>

void callback(const geometry_msgs::PoseStampedConstPtr& _msg)
{
	std::cout<<*_msg;
}

int main(int argn, char* args[])
{
	peiskmt_initialize(&argn, args);

	ros_over_peis::Subscriber <geometry_msgs::PoseStamped> sub("k.k", callback);

	int i = 10;
	while(i--)
	{
		std::cout<<"Waiting\n";
		sleep(1);
	}

	sub.shutdown();
}
