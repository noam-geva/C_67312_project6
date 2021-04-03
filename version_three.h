#include <iostream>
#include <cmath>


#define REFACTOR_RATIO 1.5
#define START_CAP 16
#define BAD_INDEX "index out of range.\n"


template<class T, size_t StaticCapacity = START_CAP>
class VLVector{

    typedef T* Iterator;
    typedef  const Iterator ConstIterator;
    typedef std::reverse_iterator<Iterator> RIterator;
    typedef const std::reverse_iterator<Iterator> RConstIterator;

    T static_data_[StaticCapacity];
    size_t size_;
    size_t cap_;
    T *data_;
    bool is_alloc_;


    /**
     *  calculates by the formula given in the exercise PDF
     * @param num_items_to_add
     * @return correct capacity
     */
    size_t CalculateCapC(const size_t num_items_to_add = 0){
        if (this->size_ + num_items_to_add > StaticCapacity){
            return floor(REFACTOR_RATIO * (this->size_ + num_items_to_add));
        }
        return StaticCapacity;
    }

    /**
     * moves the data to the heap.
     * note that the data moves before we acctually add the new items.
     * therefore we will have a place for them in the new arr.
     * @param num_items_to_add
     */
    void MoveToHeap(const size_t num_items_to_add = 0){
        this->is_alloc_ = true;
        this->cap_ = CalculateCapC(num_items_to_add);
        this->data_ = new T[this->cap_];
        this->DataCopy(&this->static_data_[0],this->size_);
    }

    /**
     * moves the data to the stack.
     */
    void MoveToStack(const size_t new_size){
        this->is_alloc_ = false;
        T *tmp = this->data_;
        this->data_ = &this->static_data_[0];
        this->cap_ = StaticCapacity;
        DataCopy(tmp, new_size);
        delete tmp; // delete the pointer holding whats still in the heap
    }

    /**
     * naive implementation.
     * copy the other data directly to this.data
     * @param other_data
     * @param data_len
     */
    void DataCopy(T *other_data, size_t data_len){
        for (size_t ix = 0 ; ix < data_len ; ix++){
            this->data_[ix] = other_data[ix];
        }
    }

    void ResizeUp(size_t num_items_to_add = 0){
        size_t new_size = this->size_ +num_items_to_add;
        if ( new_size <= StaticCapacity){
            return;
        }
        else if (new_size > StaticCapacity && !(this->is_alloc_)){
            MoveToHeap(num_items_to_add);
            return;
        }
        else {
            this->cap_ = CalculateCapC(num_items_to_add);
            T *tmp = this->data_;
            this->data_ = new T[this->cap_];
            DataCopy(tmp, this->size_);
            delete tmp; // delete the pointer holding whats still in the heap
        }
    }


public:

    /**
     * default constructor.
     */
    VLVector(){
        size_ = 0;
        cap_ = StaticCapacity;
        is_alloc_ = false;
        data_= &static_data_[0];
    }

    /**
     * copy constructor.
     * @param rhs
     */
    VLVector(const VLVector<T,StaticCapacity> &rhs){
        size_ = rhs.size_;
        cap_ = rhs.cap_;
        is_alloc_ = rhs.is_alloc_;
        if (this->is_alloc_){
            data_ = new T[this->cap_];
        } else {
            data_= &static_data_[0];
        }
        DataCopy(rhs.data_, rhs.size_);
    }

    /**
     * sequence based constructor.
     * @tparam InputIterator
     * @param first
     * @param last
     */
    template<class InputIterator>
    VLVector(InputIterator first, InputIterator last){
        size_ = 0;
        is_alloc_ = false;
        data_= &static_data_[0];
        for (; first != last; ++first) {
            if (this->size_ == StaticCapacity && !(this->is_alloc_)){
                this->MoveToHeap();
            }
            data_[this->size_] = *first;
            this->size_++;
        }
        cap_ = (is_alloc_) ? CalculateCapC() : StaticCapacity;
    }

    /**
     * single-value constructor.
     * @param count
     * @param value
     */
    VLVector(const size_t count, T value){
        if (count > StaticCapacity){
            data_ = new T[count];
            is_alloc_ = true;
            cap_ = CalculateCapC(count);
        } else {
            data_= &static_data_;
            cap_ = StaticCapacity;
            is_alloc_ = false;
        }
        size_ = count;
        for (size_t ix = 0 ; ix < count ; ix++) {
            data_[ix] = value;
        }

    }

    /**
     * destructor.
     * deletes if necessary.
     */
    ~VLVector(){
        if (this->is_alloc_){
            // delete[] this->data_;
            this->is_alloc_ = false;
        }
    }

    /**
     *
     * @return this.size_
     */
    size_t Size() const noexcept{
        return this->size_;
    }

    /**
     *
     * @return this.cap_
     */
    size_t Capacity() const noexcept{
        return this->cap_;
    }

    /**
     *
     * @return true if size == 0. if empty
     */
    bool Empty() const noexcept{
        return (size_ == 0);
    }

    /**
     * checks index validity.
     * @param index
     * @return data_[index]
     */
    T At(size_t index) const{
        if (index >= this->size_ || index < 0){
            throw std::out_of_range (BAD_INDEX);
        }
        return this->data_[index];
    }

    /**
     * adds an item at the end of the vector.
     * resize if necessary.
     * @param value of type T to add at the end.
     */
    void PushBack(const T value) noexcept {
        if (this->size_ + 1 > StaticCapacity && !(this->is_alloc_)){
            MoveToHeap();
        }
        this->data_[this->size_] = value;
        if (this->size_ == this->cap_){
            this->SizeUpCap(1);
        }
        this->size_++;
    }

    /**
     * adds an item at vector[ix] of the vector.
     * resize if necessary.
     * @param index the place to add.
     * @param value of type T to add at the end.
     * @return
     */
    Iterator Insert(Iterator it, T value) noexcept {
        if (++this->size_ > StaticCapacity && !(this->is_alloc_)){
            MoveToHeap();
        }
        this->SizeUpCap();
        for (auto iter = this->end(); iter > it ; iter--){
            *iter = *(iter - 1);
        }
        *it = value;
        return it;

    }

    /**
     * insert a sequence of type T to the vector from it to (it +(first-last +1)).
     * @tparam InputIterator iterators
     * @param it place to start adding
     * @param first
     * @param last
     * @return iterator to place
     */
    template<class InputIterator>
    Iterator Insert(Iterator it, InputIterator first, InputIterator last) noexcept {
        size_t original_size = this->size_;
        auto old_end = this->end();
        int count = 0;
        for(;first != last; first++){
            count++;
            this->PushBack((*first));
        }
        std::swap_ranges(it,old_end+1,this->end());
        if (this->is_alloc_){
            this->cap_ = floor(REFACTOR_RATIO * ((double) original_size + count));
        }
        return it;

    }

    /**
    *removes the last item im vector. resize if necessary.
    */
    void PopBack() noexcept {
        if (this->size_ == 0) {return;}
        if (--this->size_ == StaticCapacity){
            MoveToStack(this->size_);
        }
    }

    /**
     * erase a value at vector[index].
     * @param index
     * @return
     */
    Iterator Erase(Iterator it) noexcept {
        if (this->size_ == 0) {return 0;}
        for (auto iter = it ; iter < this->end() ; iter++){
            *iter = *(iter+1);
        }
        if (--this->size_ == StaticCapacity){
            MoveToStack(this->size_);
        }
        return it;
    }

    /**
     * removes a sequence from vector[it] to vector[it + first - last +1].
     * @tparam InputIterator
     * @param it
     * @param first
     * @param last
     * @return
     */
    template<class InputIterator>
    InputIterator Erase(InputIterator first, InputIterator last) noexcept {
        size_t count = 0;
        for (auto it_for_count = first; it_for_count != last ; it_for_count++){
            count++;
        }
        for (auto it = first; it != last ; it++){
            this->data_[it] = this->data_[it + count];
        }
        this->size_ -= count;
        if (this->size_<= StaticCapacity && this->is_alloc_){
            MoveToStack(this->size_);
        }
        return first;
    }

    /**
     * clears the vector.
     */
    void Clear() noexcept {
        if (this->is_alloc_){
            delete[] this->data_;
            this->is_alloc_= false;
            this->cap_ = StaticCapacity;
            this->data_ = &this->static_data_[0];
        }
        this->size_ = 0;
    }

    /**
     *returns a pointer to data, which is a pointer it self.
     */
    T* Data() const noexcept {
        return this->data_;
    }

    /**
     * const subscript operator.
     * @param index
     * @return
     */
    T operator[](const size_t index) const noexcept{
        return data_[index];
    }

    /**
     * non-const subscript operator.
     * @param index
     * @return
     */
    T& operator[](const size_t index) noexcept{
        return data_[index];
    }

    /**
     * assignment operator;
     * @param rhs
     * @return
     */
    VLVector<T, StaticCapacity>& operator=(const VLVector<T, StaticCapacity> &rhs) noexcept {
        if (this == &rhs){
            return *this;
        }
        this->size_ = rhs.size_;
        this->cap_ = rhs.cap_;
        this->is_alloc_ = rhs.is_alloc_;
        if (this->is_alloc_){
            this->data_ = new T[this->cap_];
        } else{
            data_= &static_data_;
        }
        DataCopy(rhs.data_, rhs.size_);
        return(*this);
    }

    /**
     * linear check to see if the vectors are equal
     * @param rhs
     * @return
     */
    bool operator==(const VLVector &rhs) const noexcept {
        if(size_ != rhs.size_ || cap_ != rhs.cap_){
            return false;
        }
        for (size_t ix = 0 ; ix < this->size_ ; ix++){
            if ((*this)[ix] != rhs[ix]){
                return false;
            }
        }
        return true;
    }

    /**
     *
     * @param rhs
     * @return true iff !(==)
     */
    bool operator!=(const VLVector &rhs) const noexcept {
        return *this != rhs;
    }

    /**
     * regular reverse const and const reverse begin and end
     */

    Iterator begin() const noexcept{
        return &this->data_[0];
    }

    ConstIterator cbegin() const noexcept{
        return &this->data_[0];
    }

    Iterator end() const noexcept{
        return &this->data_[this->size_];
    }

    ConstIterator cend() const noexcept{
        return &this->data_[this->size_];
    }

    RIterator rbegin() const noexcept{
        return make_reverse_iterator(&this->data_[this->size_]);
    }

    RConstIterator crbegin() const noexcept{
        return make_reverse_iterator(&this->data_[this->size_]);
    }

    RIterator rend() const noexcept{
        return make_reverse_iterator(&this->data_[0]);
    }

    RConstIterator crend() const noexcept{
        return make_reverse_iterator(&this->data_[0]);
    }

};