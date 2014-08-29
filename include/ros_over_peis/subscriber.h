#include <ros/serialization.h>
#include <pthread.h>

extern "C"
{
#include <peiskernel/peiskernel.h>
#include <peiskernel/peiskernel_mt.h>
}

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

namespace ros_over_peis
{

template <typename rosmsg_type>
class Subscriber
{
public:
	typedef void rosmsg_callback(const typename rosmsg_type::ConstPtr& p);

	void setTupleKey(std::string tuple_key)
	{
		tuple_key_ = tuple_key;
	}

	void subscribe()
	{
		//printf("!Subscribing to %s..\n", tuple_key_.c_str());

		handle_ = peiskmt_subscribe(-1, tuple_key_.c_str());
	}

	Subscriber()
	{

	}

	Subscriber(std::string tuple_key, rosmsg_callback *fn = NULL)
	{
		//printf("Subscribing to %s..\n", tuple_key.c_str());

		//puts(tuple_key.c_str());
		sleep(1);
		handle_ = peiskmt_subscribe(-1, tuple_key.c_str());

		tuple_key_ = tuple_key;
		if(fn)
		{
			callback_ = fn;
		}

		if(fn)
			pthread_create(&poll_thread_id_, NULL, poll_thread, this);

		//printf("Subscribed to %s..\n", tuple_key.c_str());
	}

	void getMsg(typename rosmsg_type::Ptr& p_ptr)
	{
		PeisTuple abstract_tuple;
		peiskmt_initAbstractTuple(&abstract_tuple);

		peiskmt_setTupleName(&abstract_tuple, this->tuple_key_.c_str());
		abstract_tuple.owner = -1;

		PeisTupleResultSet* rs;
		rs = peiskmt_createResultSet();

		peiskmt_resultSetReset(rs);

		peiskmt_getTuplesByAbstract(&abstract_tuple, rs);
		peiskmt_resultSetNext(rs);

		//printf("Searching for tuple...\n");
		PeisTuple* recd_tuple;

		for(;;peiskmt_resultSetNext(rs))
		{
			recd_tuple = peiskmt_resultSetValue(rs);
			if(!recd_tuple)
			{
				printf("Tuple doesn't appear to be published by anyone yet...\n");
				sleep(1);
				break;
			}

			// Deserialie the tuple and convert to ROS Msg
			rosmsg_type p;
			uint32_t serial_size = recd_tuple->datalen;

			ros::serialization::IStream p_stream ( (uint8_t*) recd_tuple->data, serial_size);
			ros::serialization::deserialize(p_stream, p);

			p_ptr = typename rosmsg_type::Ptr(new rosmsg_type);
			*p_ptr = p;
			printf("returning Msg\n");
			return;
		}

		p_ptr.reset();
		return;
	}

	void shutdown()
	{
		peiskmt_unsubscribe(handle_);
		pthread_cancel(poll_thread_id_);
	}

private:

	static void* poll_thread(void *_this_)
	{
		PeisTuple abstract_tuple;
		peiskmt_initAbstractTuple(&abstract_tuple);

		peiskmt_setTupleName(&abstract_tuple, ((Subscriber<rosmsg_type> *) _this_)->tuple_key_.c_str());
		abstract_tuple.owner = -1;

		PeisTupleResultSet* rs;
		rs = peiskmt_createResultSet();

		while(1)
		{
			peiskmt_resultSetReset(rs);

			// Now fetch all of doro's tuples
			peiskmt_getTuplesByAbstract(&abstract_tuple, rs);
			peiskmt_resultSetNext(rs);

			//printf("Searching for tuple...\n");
			PeisTuple* recd_tuple;

			for(;;peiskmt_resultSetNext(rs))
			{
				recd_tuple = peiskmt_resultSetValue(rs);
				if(!recd_tuple)
				{
					//printf("Tuple doesn't appear to be published by anyone yet...");
					sleep(1);
					break;
				}

				genericCallback(recd_tuple, _this_);
			}
		}
		return NULL;
	}

	static void genericCallback(PeisTuple* recd_tuple, void* _this_)
	{
		rosmsg_type p;
		uint32_t serial_size = recd_tuple->datalen;

		ros::serialization::IStream p_stream ( (uint8_t*) recd_tuple->data, serial_size);
		ros::serialization::deserialize(p_stream, p);

		typename rosmsg_type::Ptr p_ptr (new rosmsg_type);
		*p_ptr = p;
		((Subscriber<rosmsg_type> *) _this_)->callback_(p_ptr);
	}

	rosmsg_callback *callback_;
	PeisSubscriberHandle handle_;
	pthread_t poll_thread_id_;
	std::string tuple_key_;

};

}

#endif /* SUBSCRIBER_H_ */
