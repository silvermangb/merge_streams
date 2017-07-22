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
		if (!p_streams[i].get().eof()) {
			p_streams[i].get() >> v;
			if (!p_streams[i].get().fail()) {
				std::pair<int, int> e(v, i);
				merged.push(e);
			}
		}
	}

	/*
	 * get the pair, (v,i), whose first member, v, is the smallest currently in
	 * the priority queue. the second member, i, is the index of the corresponding
	 * stream.
	 *
	 * if there is another int in stream i, push the pair into the priority queue.
	 */
	bool consumed;
	while (!merged.empty()) {
		std::pair<int, int> top = merged.top();
		merged.pop();
		p_os << top.first << " " << top.second << '\n';

		std::istream & is = p_streams[top.second].get();

		if (!is.eof()) {
			consumed = false;
			is >> v;
			if (!is.fail()) {
				while (v <= merged.top().first) {
					consumed = true;
					p_os << v << " " << top.second << "\n";

					if (!is.eof()) {
						consumed = false;
						is >> v;
						if (is.fail()) {
							consumed = true;
							break;
						}
					} else {
						break;
					}
				}
				if (!consumed) {
					merged.push(std::pair<int, int>(v, top.second));
				}
			}

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

