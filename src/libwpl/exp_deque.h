/*

-------------------------------------------------------------

Copyright (c) MMXIII Atle Solbakken
atle@goliathdns.no

-------------------------------------------------------------

This file is part of P* (P-star).

P* is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

P* is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with P*.  If not, see <http://www.gnu.org/licenses/>.

-------------------------------------------------------------

*/

#pragma once

#include <cstring>

template<typename T> class wpl_exp_deque {
	private:
	T array[WPL_EXPRESSION_MAX];

	int current_pos;
	int saved_pos;

	public:
	wpl_exp_deque() {
		current_pos = -1;
	}
	wpl_exp_deque(const wpl_exp_deque<T> &copy) {
		current_pos = copy.current_pos;
		memcpy(array, copy.array, sizeof(array));
	}
	void save_pos(){
		saved_pos = current_pos;
	}
	void revert_pos() {
		current_pos = saved_pos;
	}
	void replace (int index, T &value) {
		array[index] = value;
	}
	int pos() {
		return current_pos;
	}
	void goto_pos(int pos) {
		current_pos = pos;
	}
	T &top() {
		return array[current_pos];
	}
	bool empty() {
		return (current_pos == -1);
	}
	void remove (int index) {
		for (int i = index; i <= pos; i++) {
			array[i] = array[i+1];
		}
		pos--;
	}
	void push_front (T value) {
		if (current_pos+1 >= WPL_EXPRESSION_MAX) {
			return;
		}
		for (int i = current_pos; i >= 0; i--) {
			array[i+1] = array[i];
		}
		array[0] = value;
		current_pos++;
	}
	void push(T value) {
		if (current_pos+1 >= WPL_EXPRESSION_MAX) {
			return;
		}
		array[++current_pos] = value;
	}
	void pop() {
		if (current_pos == -1) {
			return;
		}
		current_pos--;
	}
	void unpop() {
		current_pos++;
	}
	void clear() {
		current_pos = -1;
	}
	int size() {
		return current_pos+1;
	}
	T operator[] (const int &index) {
		if (index >= WPL_EXPRESSION_MAX) {
			throw runtime_error("Index out of range in wpl_exp_deque");
		}
		return array[index];
	}
};
