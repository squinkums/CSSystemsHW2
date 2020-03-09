# CSSystemsHW2
Weihang Qin
Liam Goodwin

## Choice of algorithms
The project consists of three parts----Cache, Evictor and Test. 

### Cache
The cache class is implemented in the Impl class, so we'll talk about Impl instead.

The basic underlying data structure is a vector of Link_list pointers called hashtable. We are using the Separate chaining with linked lists idiom to avoid collision. 

Link_list is a private structure of Impl that stores the key-value pairs. It only has defines a constructor and destructor. The constructor takes a key, a value and a size and deep copies them in our Link_list block. It also has a next field which points to the next Link_list. By default, next is set to nullptr. 

The other private variables we need to keep track of are:
- maxmem_: max amount of value in bytes we are allowed to store
- curmem_: current memory used
- max_load_factor_
- hasher_: our hash function
- evictor_: our evictor pointer
- cur_table_size: current hashtable size (initialized with 100, we are potentially resizing it) 
- loaded-pair: current key-value pairs loaded. If exceeded max_load_factor*cur_table_size, call the resize_hashtable method. 

There's also a private method called resize_hashtable, it takes in a multiplier and resize cur_table_size to that multiple. It works by creating a new vector, and one-by-one reinsert the key-value pairs, then delete the old table by going into each entry (the same mechanism as reset().)


The interface of Impl has the following functions: The documentation is provided by Eitan and I would not paste them here. 

- Constructor: Initialize all the values, nothing fancy here. The only thing to note is we created a null-evictor class if a nullptr is passed in for the evictor. See evictor for details.

- Destructor: clear the hashtable by going deep into each entry. delete evictor as well.

- set: First we check for the table size, if it becomes too crowded, we resize it by calling resize_hashtable(). Note that set is not only loaded with the functionality of adding elements, but also that of modifying existing element. Then there's an order in which we check 1. if the key is in the cache. 2. If the key is not in the cache, if the max memory has been exceeded. 3. If memory exceeded, if the evictor tells us something to return.  Consider if we check for memory capacity first. Then we would always evict an element first even if this set call is just a modification on an existing key. I added an additional tag and some more branches to print out if the set is called during the process of resizing the hashtable. This is purely for the purpose of debugging and could be removed. Don't forget to touch the key for the evictor and update the counters accordingly. (e.g. Do not increment curmem_ and loaded_pairs_ if it's a modification. )

- get: We need to pass the return value by copy, not just a reference. Also we only need to copy an assigned size. Don't forget to touch the key for the evictor.

- del: same searching mechanism as get. The only complication is that to delete a Link_list in the middle, we need to relink the previous list to the next list. 

- reset: clear everything in the table. Same mechanism as in resize_hashtable.  Reset the counters as well.

- space_used:  we have a counter (curmem_) for that.

- get_tablesize: solely for the purpose of debugging.



### evictor

The evictor is ab abstract class. We implement it in fifo_evictor and null_evictor

#### null_evictor
It doesn't do anything except to return empty string when asked to return the next key to evict,

#### fifo_evictor
Fifo_evictor, as the name suggested, is implemented by a deque structure. I did not choose to use a queue and allow for multiple keys because it's less intuitive and more bug prone. 

When touch_key is called, fifo checks if the key given is in the deque, if yes then we delete it. If no, We do nothing. Then we push the key to the back of the deque. 

When evict is called, it returns an empty string if the deque is empty. Otherwise it creates a copy of the first element, pops it from the deque, and returns the copy to the caller. 


### test

we wrote 9 subtests for all aspects of the project, 7 of which are for cache test, 2 of which are for evictors. Most of the tests are pretty straight-forward. The fifo_evictor test is the most general one  interms of testing for functionality. 


