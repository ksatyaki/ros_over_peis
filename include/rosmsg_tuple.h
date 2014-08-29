

namespace ros_over_peis
{
template <typename rosmsg_type>
void setROSMsgTuple(std::string tuple_name, rosmsg_type _ros_msg)
{
	uint32_t serial_size = ros::serialization::serializationLength(p);
	boost::shared_array<uint8_t> buffer(new uint8_t[serial_size]);

	std::cout<<"\nSerialization length of p is:"<<serial_size<<std::endl;

	ros::serialization::OStream p_stream (buffer.get(), serial_size);
	ros::serialization::serialize(p_stream, p);

	std::string bull_shit;


	peiskmt_setTuple("kusu.kundi",
			serial_size,
			buffer.get(), "std_msgs/String",
			PEISK_ENCODING_BINARY);
	sleep(1);
}
