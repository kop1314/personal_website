/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#include <assert.h>
#include <stdio.h>
/**
 * 'INITIAL_CAPACITY' the initial size of the dynamically.
 */
const size_t INITIAL_CAPACITY = 8;
/**
 * 'GROWTH_FACTOR' is how much the vector will grow by in automatic reallocatio
n
 * (2 means double).
 */
const size_t GROWTH_FACTOR = 2;

struct vector {
    /* The function callback for the user to define the way they want to copy
     * elements */
    copy_constructor_type copy_constructor;

    /* The function callback for the user to define the way they want to destroy
     * elements */
    destructor_type destructor;

    /* The function callback for the user to define the way they a default
     * element to be constructed */
    default_constructor_type default_constructor;

    /* Void pointer to the beginning of an array of void pointers to arbitrary
     * data. */
    void **array;

    /**
     * The number of elements in the vector.
     * This is the number of actual objects held in the vector,
     * which is not necessarily equal to its capacity.
     */
    size_t size;

    /**
     * The size of the storage space currently allocated for the vector,
     * expressed in terms of elements.
     */
    size_t capacity;
};

/**
 * IMPLEMENTATION DETAILS
 *
 * The following is documented only in the .c file of vector,
 * since it is implementation specfic and does not concern the user:
 *
 * This vector is defined by the struct above.
 * The struct is complete as is and does not need any modifications.
 *
 * The only conditions of automatic reallocation is that
 * they should happen logarithmically compared to the growth of the size of the
 * vector inorder to achieve amortized constant time complexity for appending to
 * the vector.
 *
 * For our implementation automatic reallocation happens when -and only when-
 * adding to the vector makes its new  size surpass its current vector capacity
 * OR when the user calls on vector_reserve().
 * When this happens the new capacity will be whatever power of the
 * 'GROWTH_FACTOR' greater than or equal to the target capacity.
 * In the case when the new size exceeds the current capacity the target
 * capacity is the new size.
 * In the case when the user calls vector_reserve(n) the target capacity is 'n'
 * itself.
 * We have provided get_new_capacity() to help make this less ambigious.
 */

static size_t get_new_capacity(size_t target) {
    /**
     * This function works according to 'automatic reallocation'.
     * Start at 1 and keep multiplying by the GROWTH_FACTOR untl
     * you have exceeded or met your target capacity.
     */
    size_t new_capacity = 1;
    while (new_capacity < target) {
        new_capacity *= GROWTH_FACTOR;
    }
    return new_capacity;
}

vector *vector_create(copy_constructor_type copy_constructor,
                      destructor_type destructor,
                      default_constructor_type default_constructor) {
    // your code here
   vector *v = malloc(sizeof(vector));
    v -> copy_constructor = copy_constructor;
    v -> destructor = destructor;
    v -> default_constructor = default_constructor;
    v -> array = (void**)calloc(INITIAL_CAPACITY, sizeof(void*));
      v -> size = 0;
      v -> capacity = INITIAL_CAPACITY;
    
    // Casting to void to remove complier error. Remove this line when you are
    // ready.
    //(void)INITIAL_CAPACITY;
    //(void)get_new_capacity;
    return v;
}

void vector_destroy(vector *this) {
  // printf("start destroy");
    assert(this);
    // your code here
    size_t index = 0;
    destructor_type destruct = this -> destructor;
    //printf("destroy loop\n");
    while(index < (this -> capacity)) {
      destruct(this -> array[index]);
      //this -> array[index] = NULL;
      index++;
    }
    //printf("free this->array\n");
    free(this->array);
    //printf("free this\n");
    free(this);
    return;
}

void **vector_begin(vector *this) {
    return this->array + 0;
}

void **vector_end(vector *this) {
    return this->array + this->size;
}

size_t vector_size(vector *this) {
    assert(this);
    // your code here
    //return 0;
    return this->size;
}

void vector_resize(vector *this, size_t n) {
  //printf("start");
  assert(this);
    // your code here
    if (n < this -> size){
      // this -> size = n;
      size_t index = n;
      destructor_type destruct = this -> destructor;
      while(index < this -> size){
	  destruct(this->array[index]);
	  index++;
      }
      this -> size = n;
    }
    if (n > this -> size) {
      if(n > this -> capacity){
	vector_reserve(this, n);
      }
      //printf("pass if n >capa");
      size_t index = this -> size;
      default_constructor_type default_construct = this -> default_constructor;
      //printf("while loop");
      while(index < n){
	//void *ptr = default_construct();
	//this -> array[index] = ptr;
	this ->array[index] = default_construct();
	index++;
      }
      this -> size = n;
    }
}

size_t vector_capacity(vector *this) {
    assert(this);
    // your code here
    //return 0;
    return this->capacity;
}

bool vector_empty(vector *this) {
    assert(this);
    // your code here
    return (this->size==0);
}

void vector_reserve(vector *this, size_t n) {
    assert(this);
    // your code
    if(n > this -> capacity){
      size_t oldSize = this->capacity;
      this->capacity = get_new_capacity(n);
      void** temp = (void**)realloc(this->array, (this->capacity) *sizeof(void*));
       if(temp)
	{
	  //printf("I get memomery!");
	  this -> array = temp;
	}
      for (size_t i = oldSize; i < this->capacity; i++)
      {
	  this->array[i] = NULL;
      }
      //this->array = (void**)realloc(this->array, (this->capacity) *sizeof(void*));
      //return;
    }
}

void **vector_at(vector *this, size_t position) {
    assert(this);
    assert(position < this->size);
    // your code here
    // void ** ref = (void**)malloc(sizeof(void*));
     //ref = this->array[position];
    return this->array + position;
    //return ref;
}
void vector_set(vector *this, size_t position, void *element) {
    assert(this);
    assert(position < this->size);
    // your code here
      if(element ==NULL) {
	this->array[position] = NULL;
      } else {
	destructor_type destruct = this->destructor;
	destruct(this->array[position]);
	this->array[position] = this->copy_constructor(element);
      }
}

void *vector_get(vector *this, size_t position) {
    assert(this);
    assert(position < this->size);
    // your code here
      if(position >= this->size){
	return NULL;
      }
      return this->array[position];
}

void **vector_front(vector *this) {
    assert(this);
    return this->array+0;
}

void **vector_back(vector *this) {
    // your code here
    assert(this);
    return this->array + this->size - 1;
}

void vector_push_back(vector *this, void *element) {
    assert(this);
    // your code here
    if(this -> size > this -> capacity) {
      return;
    }
    else if(this -> size == this -> capacity) {
      copy_constructor_type copy_construct = this -> copy_constructor;
      vector_reserve(this, this -> size + 1);
      //this -> array = (void**)realloc(this -> array, this -> size + 1);
      this -> array[this -> size] = copy_construct(element);
      this -> size = this -> size + 1;
      //this -> capacity = this -> capacity + 1;
      return;
    } else {
       copy_constructor_type copy_construct = this -> copy_constructor;
       //this -> array = (void**)realloc(this -> array, this -> size + 1);
      this -> array[this -> size] = copy_construct(element);
      this -> size = this -> size + 1;
    }
    
}

void vector_pop_back(vector *this) {
    assert(this);
    // your code here
    if(this -> size == 0){
      return;
    }
    destructor_type destruct = this -> destructor;
    destruct(this -> array[this->size - 1]);
    this -> size = this -> size - 1;
}

void vector_insert(vector *this, size_t position, void *element) {
    assert(this);
    //my code
    //assert(position <= this -> size);
    // your code here
    if(position > this -> size) {
      return;
    }
    if(this -> size == this -> capacity) {
      if(position == this -> size){
	 copy_constructor_type copy_construct = this -> copy_constructor;
	 vector_reserve(this, this -> size + 1);
	 //this -> array = (void**)realloc(this -> array, this -> size + 1);
	 this -> array[this -> size] = copy_construct(element);
	 this -> size = this -> size + 1;
	 //this -> capacity = this -> capacity + 1;
	 return;
      }
       copy_constructor_type copy_construct = this -> copy_constructor;
       vector_reserve(this, this -> size + 1);
       //this -> array = (void**)realloc(this -> array, this -> size + 1);
       size_t index = this -> size;
       while(index > position)
	 {
	   this -> array[index] = this -> array[index - 1];
	   index--;
	 }
       this -> array[index] = copy_construct(element);
       this -> size = this -> size + 1;
       //this -> capacity = this -> capacity + 1;
       return;
    }
    if(position == this -> size){
      copy_constructor_type copy_construct = this -> copy_constructor;
      //this -> array = (void**)realloc(this -> array, this -> size + 1);
	 this -> array[this -> size] = copy_construct(element);
	 this -> size = this -> size + 1;
	 return;
    }
     copy_constructor_type copy_construct = this -> copy_constructor;
     //this -> array = (void**)realloc(this -> array, this -> size + 1);
       size_t index = this -> size;
       while(index > position)
	 {
	   this -> array[index] = this -> array[index - 1];
	   index--;
	 }
       this -> array[index] = copy_construct(element);
       this -> size = this -> size + 1;
   
       return;
}

void vector_erase(vector *this, size_t position) {
    assert(this);
    assert(position < vector_size(this));
    // your code here
    size_t index = position;
    //size_t thisSize = this -> size;
    destructor_type destruct = this -> destructor;
    //copy_constructor_type copy_construct = this -> copy_constructor;
    destruct(this -> array[position]);
    while(index < this -> size - 1) {
      this -> array[index] = this -> array[index+1];
      index ++;
    }
    this -> array[this -> size] = NULL;
    this -> size = this -> size - 1;
    //vector_resize(this, thisSize - 1);
}

void vector_clear(vector *this) {
    // your code here
  assert(this);
   size_t index = 0;
    destructor_type destruct = this -> destructor;
    while(index < (this -> size)) {
      destruct(this -> array[index]);
       index++;
    }
    this -> size = 0;
    //free(this->array);
}

// The following is code generated:
vector *shallow_vector_create() {
    return vector_create(shallow_copy_constructor, shallow_destructor,
                         shallow_default_constructor);
}
vector *string_vector_create() {
    return vector_create(string_copy_constructor, string_destructor,
                         string_default_constructor);
}
vector *char_vector_create() {
    return vector_create(char_copy_constructor, char_destructor,
                         char_default_constructor);
}
vector *double_vector_create() {
    return vector_create(double_copy_constructor, double_destructor,
                         double_default_constructor);
}
vector *float_vector_create() {
    return vector_create(float_copy_constructor, float_destructor,
                         float_default_constructor);
}
vector *int_vector_create() {
    return vector_create(int_copy_constructor, int_destructor,
                         int_default_constructor);
}
vector *long_vector_create() {
    return vector_create(long_copy_constructor, long_destructor,
                         long_default_constructor);
}
vector *short_vector_create() {
    return vector_create(short_copy_constructor, short_destructor,
                         short_default_constructor);
}
vector *unsigned_char_vector_create() {
    return vector_create(unsigned_char_copy_constructor,
                         unsigned_char_destructor,
                         unsigned_char_default_constructor);
}
vector *unsigned_int_vector_create() {
    return vector_create(unsigned_int_copy_constructor, unsigned_int_destructor,
                         unsigned_int_default_constructor);
}
vector *unsigned_long_vector_create() {
    return vector_create(unsigned_long_copy_constructor,
                         unsigned_long_destructor,
                         unsigned_long_default_constructor);
}
vector *unsigned_short_vector_create() {
    return vector_create(unsigned_short_copy_constructor,
                         unsigned_short_destructor,
                         unsigned_short_default_constructor);
}

