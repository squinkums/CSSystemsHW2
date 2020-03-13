#include <iostream>
#include "cache.hh"
#include "fifo_evictor.h"
#include <cassert>
#include <chrono>
#include <thread>


void basic_test()
{
	//A basic test that tests for basic set() and get() functionality
	//Create a new cache 
	auto my_cache = new Cache(100);
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 2;
	//Test set
	my_cache->set(my_key1, my_value1, 3);
	//Test get
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'a');
	assert(get_back[1] == 'b');
	// We have only gotten back 2 bytes, the third byte should not match
	assert(get_back[2] != 'c');
	assert(my_cache->space_used() == 3);
	delete[] get_back;
	delete my_cache;
	
	delete[] my_value1;
	return;
}

size_t liam_hash(key_type key)
{
	// A hash function that forces "ab" and "ba" to collide in the hashtable
	int result = 0;
	for (auto x : key)
	{
		int mod = x % 10;
		result += mod;
	}
	return result;
}


void Collision_test()
{
	// Map two keys into the same hashtable entry 
	auto my_cache = new Cache(100,0.75,nullptr,liam_hash);
	
	key_type const my_key1 = "ab";
	key_type const my_key2= "ba";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	auto const my_value2 = new char[3];
	my_value2[0] = 'd';
	my_value2[1] = 'e';
	my_value2[2] = 'f';
	uint32_t my_size = 3;
	my_cache->set(my_key1, my_value1, 3);
	my_cache->set(my_key2, my_value2, 3);
	delete[] my_value1;
	delete[] my_value2;
	const auto get1 = my_cache->get(my_key1, my_size);
	const auto get2 = my_cache->get(my_key2, my_size);
	assert(get1[0] == 'a');
	assert(get2[0] == 'd');
	delete my_cache;
	delete[] get1;
	delete[] get2;
	return;
}


void deep_copy_test()
{
	// Test if the values are deep copied
	auto my_cache = new Cache(100);
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 2;
	my_cache->set(my_key1, my_value1, 3);
	//The value is deleted
	delete[] my_value1;
	//Since the values are deep copied, we should still be able to get back our values.
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'a');
	assert(get_back[1] == 'b');
	assert(get_back[2] != 'c');
	// The cache is deleted
	delete my_cache;
	// Since the return value of get is deep-copied, we should still be able to access it
	assert(get_back[0] == 'a');
	delete[] get_back;
	

	return;
	
}

void deletion_test()
{
	// Try to delete an element
	auto my_cache = new Cache(100);
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 3;
	my_cache->set(my_key1, my_value1, 3);
	auto res = my_cache->del(my_key1);
	assert(res == true);
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back == nullptr);
	assert(my_cache->del(my_key1) == false);
	delete my_cache;
	delete[] get_back;
	delete[] my_value1;
	return;
	
}




void overwrite_test()
{
	auto my_cache = new Cache(100);
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	auto const my_value2 = new char[3];
	my_value2[0] = 'd';
	my_value2[1] = 'e';
	my_value2[2] = 'f';
	my_cache->set(my_key1, my_value1, 3);
	uint32_t my_size = 2;
	my_cache->set(my_key1, my_value2, 3);
	auto const get_back = my_cache->get(my_key1, my_size);
	assert(get_back[0] == 'd');
	delete my_cache;
	delete[] get_back;
	delete[] my_value1;
	delete[] my_value2;
	return;
}




void reset_test()
{
	auto my_cache = new Cache(100);
	key_type const my_key1 = "hello";
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	uint32_t my_size = 3;
	my_cache->set(my_key1, my_value1, my_size);
	my_cache->reset();
	assert(my_cache->space_used() == 0);
	delete my_cache;

	delete[] my_value1;
	return;
}

void resize_test()
{
	auto my_cache = new Cache(400);
	for(auto i = 0;i<100;i++)
	{
		key_type const my_key1 = std::to_string(i);
		auto const my_value1 = new char[3];
		my_value1[0] = 'a';
		my_value1[1] = 'b';
		my_value1[2] = 'c';
		my_cache->set(my_key1, my_value1, 3);
		delete[] my_value1;
	}
	assert(my_cache->space_used() == 300);
	delete my_cache;

}

void leak_test()
{
	
	auto my_cache = new Cache(30000);
	auto const my_value1 = new char[3];
	my_value1[0] = 'a';
	my_value1[1] = 'b';
	my_value1[2] = 'c';
	for (auto i = 0; i < 7500; i++)
	{
		key_type const my_key1 = std::to_string(i);

		my_cache->set(my_key1, my_value1, 3);
	}
	delete[] my_value1;
	delete my_cache;
	return;
	
	//auto my_cache = new Cache(30000);
	//auto const my_value1 = new char[30000];
	//for(auto i = 0;i<30000;i++)
	//{
	//	my_value1[i] = 'a';
	//}
	//my_cache->set("ab", my_value1, 3);
	//delete[] my_value1;
	//delete my_cache;;
}


void null_evictor_test()
{
	auto my_cache = new Cache(100);
	for (auto i = 0; i < 50; i++)
	{
		key_type const my_key1 = std::to_string(i);
		auto const my_value1 = new char[3];
		my_value1[0] = 'a';
		my_value1[1] = 'b';
		my_value1[2] = 'c';
		my_cache->set(my_key1, my_value1, 3);
		delete[] my_value1;
	}
	assert(my_cache->space_used() == 99);
	delete my_cache;
	
	

}


void fifo_evictor_test()
{
	uint32_t my_size = 3;
	auto my_cache = new Cache(150,075,new Fifo_evictor(), std::hash<key_type>());
	for (auto i = 0; i < 50; i++)
	{
		key_type const my_key1 = std::to_string(i);
		auto const my_value1 = new char[my_size];
		my_value1[0] = 'a';
		my_value1[1] = 'b';
		my_value1[2] = 'c';
		my_cache->set(my_key1, my_value1, my_size);
		delete[] my_value1;
	}
	key_type const my_key1 = std::to_string(50);
	auto const my_value1 = new char[3];
	my_value1[0] = 'd';
	my_value1[1] = 'e';
	my_value1[2] = 'f';
	my_cache->set(my_key1, my_value1, 3);
	auto const get0 = my_cache->get(std::to_string(0), my_size);
	assert(get0 == nullptr);
	auto const get50 = my_cache->get(my_key1, my_size);
	assert(get50[0] == 'd');
	assert(my_cache->space_used() == 150);
	key_type const my_key2 = std::to_string(51);
	my_cache->set(std::to_string(1), my_value1, 3);
	my_cache->set(my_key2, my_value1, 3);
	auto const get2 = my_cache->get(std::to_string(2), my_size);
	assert( get2 == nullptr);
	auto const get1 = my_cache->get(std::to_string(1), my_size);
	assert(get1[0] == 'd');
	assert(my_cache->space_used() == 150);
	delete[] get0;
	delete[] get50;
	delete[] get2;
	delete[] get1;
	delete[] my_value1;
	delete my_cache;


}

int main()
{
	//Basic test
	std::cout << "---------------------------------basic test started---------------------------------------"<<'\n' << '\n';
	basic_test();
	std::cout << "basic test passed!" << '\n';
	//test for deep copy
	std::cout << "---------------------------------deep copy test started---------------------------------------" << '\n' << '\n';
	deep_copy_test();
	std::cout << "deep copy test passed!" << '\n';

	// Test for deletion
	std::cout << "---------------------------------deletion test started---------------------------------------" << '\n' << '\n';
	deletion_test();
	std::cout << "deletion test passed!" << '\n';


	// Test for overwrite
	std::cout << "---------------------------------overwrite test started---------------------------------------" << '\n' << '\n';
	overwrite_test();
	std::cout << "overwrite test passed!" << '\n';
	
	//Test for reset
	std::cout << "---------------------------------reset test started---------------------------------------" << '\n' << '\n';
	reset_test();
	std::cout << "reset test passed!" << '\n';
	
	//Test for collision
	std::cout << "---------------------------------collision test started---------------------------------------" << '\n' << '\n';

	Collision_test();
	std::cout << "collision test passed!" << '\n';
	
	//test for resizing
	//Because of the restriction of Cache interface we cannot write an assert. See the console output to check for correctness.
	std::cout << "---------------------------------resize test started---------------------------------------" << '\n' << '\n';
	resize_test();


	//Test for null_evictor
	std::cout << "---------------------------------null evictor test started---------------------------------------" << '\n' << '\n';
	null_evictor_test();
	std::cout << "null evictor test passed!" << '\n';
	//Test for fifo_evictor
	std::cout << "---------------------------------fifo evictor test started---------------------------------------" << '\n' << '\n';
	fifo_evictor_test();
	std::cout << "fifo evictor test passed!" << '\n';
	

	//leak_test();


	std::cout << "---------------------------------all tests passed :>--------------------------------------" << '\n' << '\n';



	return 0; 
}

