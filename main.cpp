/*
 * merge_streams.cpp
 *
 *  Created on: Jun 29, 2017
 *      Author: Greg Silverman
 *
 *  function merge_streams accepts a vector of references to istreams and
 *  a reference to an ostream. the merged input streams are streamed to the ostream.
 */

#include <functional>
#include <algorithm>
#include <queue>
#include <vector>
#include <iostream>
#include <fstream>


std::ostream&
operator<<(std::ostream& os, std::pair<int, int> const & p) {
	os << '(' << p.first << ',' << p.second << ')';
	return os;
}

/**
 * get the next value T in an istream.
 *
 * returns:
 * 	true if a value of type T was found.
 * 	false if no value was found
 */
template<typename T>
bool next_from_stream
	(std::istream & p_is,
	 T& p_t)
{
	if(!p_is.eof())
	{
		p_is >> p_t;
		if(!p_is.fail())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
/**
 * use a priority queue to select the next stream to read from and write to the output,
 * p_os. the priority queue contains pairs of integers, (v,i), where v is the last value read from
 * stream i in the vector of streams.
 */
void merge_streams(
		std::vector<std::reference_wrapper<std::istream>> & p_streams,
		std::ostream& p_os) {

	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,std::greater<std::pair<int,int>>> merged;

	// initialize
	int v;
	for (size_t i = 0; i < p_streams.size(); ++i) {
		if(next_from_stream<>(p_streams[i].get(),v))
		{
			std::pair<int, int> e(v, i);
			merged.push(e);
		}
	}

	/*
	 * get the pair, (v,i), whose first member, v, is the smallest currently in
	 * the priority queue. the second member, i, is the index of the corresponding
	 * stream.
	 */
	bool unconsumed_value;
	while (!merged.empty()) {

		std::pair<int, int> top = merged.top();
		merged.pop();
		p_os << top.first << " " << top.second << '\n';

		std::istream & current_stream = p_streams[top.second].get();

		/**
		 * continue to read from current stream while its next value
		 * is less than or equal to the smallest value in the priority queue.
		 */
		if((unconsumed_value=next_from_stream<>(current_stream,v)))
		{
			while(v<=merged.top().first)
			{
				unconsumed_value = false;
				p_os << v << " " << top.second << "\n";
				if(!(unconsumed_value=next_from_stream<>(current_stream,v)))
				{
					break;
				}
			}
		}
		if(unconsumed_value)
		{
			merged.push(std::pair<int, int>(v, top.second));
		}

	}

}

/**
 * merge 3 sorted files into std::cout.
 */
int main() {

	std::vector<std::reference_wrapper<std::istream>> vss;

	std::ifstream a("a.txt");
	std::ifstream b("b.txt");
	std::ifstream c("c.txt");

	vss.push_back(a);
	vss.push_back(b);
	vss.push_back(c);

	merge_streams(vss, std::cout);

	return 0;
}

