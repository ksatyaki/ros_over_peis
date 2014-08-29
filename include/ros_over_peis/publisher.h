/*
 * publisher.h
 * The ROS-over-peis publisher class.
 *  Created on: Jun 4, 2014
 *      Author: Chittaranjan S Srinivas
 */
#include <ros/serialization.h>

extern "C"
{
#include <peiskernel/peiskernel.h>
#include <peiskernel/peiskernel_mt.h>
}

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

namespace ros_over_peis
{

template <typename rosmsg_type>
class Publisher
{
public:
	Publisher(const char* tuple_key)
	{
		_tuple_key = tuple_key;
	}

	void setTupleKey(std::string tuple_key)
	{
		_tuple_key = tuple_key;
	}

	void publish(rosmsg_type msg)
	{
		uint32_t serial_size = ros::serialization::serializationLength(msg);
		boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);

		std::cout<<"\nSerialization length of p is:"<<serial_size<<std::endl;

		ros::serialization::OStream p_stream (buffer.get(), serial_size);
		ros::serialization::serialize(p_stream, msg);

		peiskmt_setTuple(
						_tuple_key.c_str(),
						serial_size,
						buffer.get(),
						"ros_msg",
						PEISK_ENCODING_BINARY);
			//std::cout<<"SUCCESS";
		sleep(1);
	}

private:
	std::string _tuple_key;
};

}
#endif /* PUBLISHER_H_ */
