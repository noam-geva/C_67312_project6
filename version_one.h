//
// Created by noamg on 11/01/2021.
//

#include <iostream>


#define START_CAP 16
#define BAD_INDEX "index out of range.\n"

template<class T, int StaticCapacity = START_CAP>
class VLVector{
    size_t size_;
    size_t cap_;
    bool is_alloc_;
    T *data_;
    T static_data_[StaticCapacity];

    /**
     * simple helper function to save code duplication
     * @param num_items_to_add
     */
    void SizeUpCap(const size_t num_items_to_add = 0){
        if (this->is_alloc_){
            this->cap_ = CalculateCapC();
        }
    }

    /**
     *  calculates by the formula given in the exercise PDF
     * @param num_items_to_add
     * @return correct capacity
     */
    size_t CalculateCapC(const size_t num_items_to_add = 0){
        if (this->size_ + num_items_to_add > StaticCapacity){
            return floor(1.5*(this->size_ + num_items_to_add));
        }
        return StaticCapacity;
    }

    /**
     * moves the data to the heap.
     * @param num_items_to_add
     */
    void MoveToHeap(const size_t num_items_to_add = 0){
        this->is_alloc_ = true;
        T *tmp = this->data_;
        this->cap_ = CalculateCapC(num_items_to_add);
        this->data_ = new T[this->cap_];
        this->DataCopy(tmp,this->size_);
    }

    /**
     * moves the data to the stack.
     */
    void MoveToStack(const size_t new_size){
        this->is_alloc_ = false;
        T *tmp = this->data_;
        this->data_ = &this->static_data_;
        this->cap_ = StaticCapacity;
        DataCopy(tmp, new_size);
        delete tmp;
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



public:

    /**
     * default constructor.
     */
    VLVector(){
        size_ = 0;
        cap_ = StaticCapacity;
        is_alloc_ = false;
        data_= &static_data_;
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
            data_= &static_data_;
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
        data_= &static_data_;
        for (; first != last; ++first) {
            data_[this->size_] = *first;
            this->size_++;
            if (this->size_ > StaticCapacity && !(this->is_alloc_)){
                this->MoveToHeap();
            }
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
            cap_ = CalculateCapC();
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
            delete this->data_;
            this->is_alloc_ = false;
        }
    }

    /**
     *
     * @return this.size_
     */
    size_t Size() const noexcept{
        return  this->size_;
    }

    /**
     *
     * @return this.cap_
     */
    size_t capacity() const noexcept{
        return  this->cap_;
    }

    /**
     *
     * @return true if size == 0. if empty
     */
    bool Empty() const noexcept{
        return size_ == 0;
    }

    /**
     * checks index validity.
     * @param index
     * @return data_[index]
     */
    T At(int index) const{
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
        if (++this->size_ > StaticCapacity){
            MoveToHeap();
        }
        this->data_[this->size_] = value;
        this->SizeUpCap(1);
    }

    /**
     * adds an item at vector[ix] of the vector.
     * resize if necessary.
     * @param index the place to add.
     * @param value of type T to add at the end.
     * @return
     */
    size_t Insert(const size_t index, T value) noexcept {
        if (++this->size_ > StaticCapacity && (this->is_alloc_ == false)){
            MoveToHeap(1);
        }
        this->SizeUpCap(1);
        for (size_t ix = this->size_; ix > index ; ix--){
            this->data_[ix] = this->data_[ix - 1];
        }
        this[index] = value;
        return index;

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
    InputIterator Insert(InputIterator it, InputIterator first, InputIterator last) noexcept {
        size_t count = 0;
        for (auto it_for_count = first; it_for_count != last ; it++){
            count++;
        }
        if (this->size_ + count > StaticCapacity && (this->is_alloc_ == false)){
            MoveToHeap(count);
        }
        this->SizeUpCap(count);
        for (size_t ix = this->size_ + count; ix > last ; ix--){
            this->data_[ix] = this->data_[ix - count];
        }
        for (; first != last ; first++) {
            this[it] = *first;
            it++;
        }
        this->size_ += count;
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
    size_t Erase(const size_t index) noexcept {
        if (this->size_ == 0) {return 0;}
        for (size_t ix = index ; ix < this->size_ ; ix++){
            this->data_[ix] = this->data_[ix+1];
        }
        if (--this->size_ == StaticCapacity){
            MoveToStack(this->size_);
        }
        return index;
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
        if (this->size_<= StaticCapacity && (this->is_alloc_ == true)){
            MoveToStack(this->size_);
        }
        return first;
    }

    /**
     * clears the vector.
     */
    void Clear() noexcept {
        if (this->is_alloc_){
            delete [] this->data_;
            this->is_alloc_== false;
            this->cap_ = StaticCapacity;
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
        return &data_[index];
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
        if(size_ == rhs.size_ && cap_ == rhs.cap_){
            return false;
        }
        for (size_t ix = 0 ; ix < this->data_ ; ix++){
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

    template<class input_iterator>
    input_iterator begin() const noexcept{
        return Iterator(&this->data_[0]);
    }

    template<class const_input_iterator>
    const_input_iterator cbegin() const noexcept{
        return Iterator(&this->data_[0]);
    }

    template<class reverse_input_iterator>
    reverse_input_iterator rbegin() const noexcept{
        return Iterator(&this->data_[0]);
    }

    template<class const_reverse_input_iterator>
    const_reverse_input_iterator crbegin() const noexcept{
        return Iterator(&this->data_[0]);
    }

    template<class input_iterator>
    input_iterator end() const noexcept{
        return Iterator(&this->data_[this->size_]);
    }

    template<class const_input_iterator>
    const_input_iterator cend() const noexcept{
        return Iterator(&this->data_[this->size_]);
    }

    template<class reverse_input_iterator>
    reverse_input_iterator rend() const noexcept{
        return Iterator(&this->data_[this->size_]);
    }

    template<class const_reverse_input_iterator>
    const_reverse_input_iterator crend() const noexcept{
        return Iterator(&this->data_[this->size_]);
    }


    /**=============================================================================================*/


    class ConstIterator;
    /**
     * RIterator class
     */
    class Iterator {
        friend class ConstIterator;
        T *value_ptr_;

    public:
        typedef T value_type;
        typedef T &reference;
        typedef T *pointer;
        typedef std::ptrdiff_t difference_type;
        typedef std::random_access_iterator_tag iterator_category;

        Iterator(const T *it ) : value_ptr_(it) {}

        Iterator &operator++() {
            this->value_ptr_++;
            return *this;
        }

        Iterator operator++(int){
            Iterator it(this->value_ptr_);
            this->value_ptr_++;
            return it;
        }

        Iterator &operator--(){
            this->value_ptr_--;
            return *this;
        }

        Iterator operator--(int){
            Iterator it(this->value_ptr_);
            this->value_ptr_--;
            return it;
        }

        Iterator &operator+(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        Iterator operator-(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        Iterator &operator+(const Iterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        Iterator operator-(const Iterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        Iterator &operator+=(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        Iterator operator-=(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        Iterator &operator+=(const Iterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        Iterator operator-=(const Iterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }


        bool operator==(const Iterator &rhs) const {
            return this->value_ptr_ == rhs.value_ptr_;
        }

        bool operator!=(const Iterator &rhs) const {
            return this->value_ptr_ != rhs.value_ptr_;
        }

        reference operator*() const{
            return *this->value_ptr_;
        }

        pointer operator->() const{
            return &(operator*());
        }

        bool operator<(const Iterator &rhs) const {
            return this->value_ptr_ < rhs.value_ptr_;
        }

        bool operator>(const Iterator &rhs) const {
            return this->value_ptr_ > rhs.value_ptr_;
        }

        bool operator<=(const Iterator &rhs) const {
            return this->value_ptr_ <= rhs.value_ptr_;
        }

        bool operator>=(const Iterator &rhs) const {
            return this->value_ptr_ >= rhs.value_ptr_;
        }
    };


    /**
     * const iterator
     */
    class ConstIterator {
        T* value_ptr_;

    public:
        typedef T value_type;
        typedef const T &reference; // note the difference between the const and non-const
        typedef const T *pointer; // note the difference between the const and non-const
        typedef std::ptrdiff_t difference_type; // irrelevant here, as we have no difference - but
        // still required
        typedef std::forward_iterator_tag iterator_category;

        ConstIterator(const T* ptr ) : value_ptr_(ptr) {}

        // Conversion ctor
        ConstIterator(const Iterator &it) : value_ptr_(it.value_ptr_) {}

        ConstIterator &operator++() const {
            this->value_ptr_++;
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator it(this->value_ptr_);
            this->value_ptr_++;
            return it;
        }

        ConstIterator &operator--(){
            this->value_ptr_--;
            return *this;
        }

        ConstIterator operator--(int){
            ConstIterator it(this->value_ptr_);
            this->value_ptr_--;
            return it;
        }

        bool operator==(const ConstIterator &rhs) const {
            return this->value_ptr_ == rhs.value_ptr_;
        }

        bool operator!=(const ConstIterator &rhs) const {
            return this->value_ptr_ != rhs.value_ptr_;
        }
        value_type operator*(){
            return *this->value_ptr_;
        }


        pointer operator->(){
            return &(operator*());
        }

        ConstIterator &operator+(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        ConstIterator operator-(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        ConstIterator &operator+(const ConstIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        ConstIterator operator-(const ConstIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        ConstIterator &operator+=(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        ConstIterator operator-=(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        ConstIterator &operator+=(const ConstIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        ConstIterator operator-=(const ConstIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        bool operator<(const ConstIterator &rhs) const {
            return this->value_ptr_ < rhs.value_ptr_;

        }

        bool operator>(const ConstIterator &rhs) const {
            return this->value_ptr_ > rhs.value_ptr_;
        }

        bool operator<=(const ConstIterator &rhs) const {
            return this->value_ptr_ <= rhs.value_ptr_;
        }

        bool operator>=(const ConstIterator &rhs) const {
            return this->value_ptr_ >= rhs.value_ptr_;
        }


    };

    class RConstIterator;
    /**
     * RIterator class
     */
    class RIterator {
        friend class RConstIterator;
        T *value_ptr_;

    public:
        typedef T value_type;
        typedef T &reference;
        typedef T *pointer;
        typedef std::ptrdiff_t difference_type;
        typedef std::random_access_iterator_tag iterator_category;

        RIterator(const T *it ) : value_ptr_(it) {}

        /**
         * inc/dec operators
         */

        RIterator &operator++() {
            this->value_ptr_--;
            return *this;
        }

        RIterator operator++(int){
            RIterator it(this->value_ptr_);
            this->value_ptr_--;
            return it;
        }

        RIterator &operator--(){
            this->value_ptr_++;
            return *this;
        }

        RIterator operator--(int){
            RIterator it(this->value_ptr_);
            this->value_ptr_++;
            return it;
        }

        /**
         * commutative operators
         */

        RIterator &operator+(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        RIterator operator-(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        RIterator &operator+(const RIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        RIterator operator-(const RIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        RIterator &operator+=(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        RIterator operator-=(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        RIterator &operator+=(const RIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        RIterator operator-=(const RIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        /**
         * equality operators
         */

        bool operator==(const RIterator &rhs) const {
            return this->value_ptr_ == rhs.value_ptr_;
        }

        bool operator!=(const RIterator &rhs) const {
            return this->value_ptr_ != rhs.value_ptr_;
        }

        /**
         * dereference operators
         */

        reference operator*() const{
            return *this->value_ptr_;
        }

        pointer operator->() const{
            return &(operator*());
        }

        /**
         * rations operators
         */

        bool operator<(const RIterator &rhs) const {
            return this->value_ptr_ > rhs.value_ptr_;
        }

        bool operator>(const RIterator &rhs) const {
            return this->value_ptr_ < rhs.value_ptr_;
        }

        bool operator<=(const RIterator &rhs) const {
            return this->value_ptr_ >= rhs.value_ptr_;
        }

        bool operator>=(const RIterator &rhs) const {
            return this->value_ptr_ >= rhs.value_ptr_;
        }
    };


    /**
     * reverse const iterator
     */
    class RConstIterator {
        T* value_ptr_;

    public:
        typedef T value_type;
        typedef const T &reference;
        typedef const T *pointer;
        typedef std::ptrdiff_t difference_type;
        typedef std::random_access_iterator_tag iterator_category;

        RConstIterator(const T* ptr ) : value_ptr_(ptr) {}

        // Conversion ctor
        RConstIterator(const RIterator &it) : value_ptr_(it.value_ptr_) {}

        /**
         * inc/dec operators
         */

        RConstIterator &operator++() const {
            this->value_ptr_--;
            return *this;
        }

        RConstIterator operator++(int) {
            RConstIterator it(this->value_ptr_);
            this->value_ptr_--;
            return it;
        }

        RConstIterator &operator--(){
            this->value_ptr_++;
            return *this;
        }

        RConstIterator operator--(int){
            RConstIterator it(this->value_ptr_);
            this->value_ptr_++;
            return it;
        }

        /**
         * equality operators
         */

        bool operator==(const RConstIterator &rhs) const {
            return this->value_ptr_ == rhs.value_ptr_;
        }

        bool operator!=(const RConstIterator &rhs) const {
            return this->value_ptr_ != rhs.value_ptr_;
        }

        /**
         * dereference operators
         */

        value_type operator*(){
            return *this->value_ptr_;
        }


        pointer operator->(){
            return &(operator*());
        }

        /**
         * commutative operators
         */

        RConstIterator &operator+(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        RConstIterator operator-(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        RConstIterator &operator+(const RConstIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        RConstIterator operator-(const RConstIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        RConstIterator &operator+=(const int num){
            this->value_ptr_ -= num;
            return *this;
        }

        RConstIterator operator-=(const int num){
            this->value_ptr_ += num;
            return *this;
        }

        RConstIterator &operator+=(const RConstIterator it){
            this->value_ptr_ -= it.value_ptr_;
            return *this;
        }

        RConstIterator operator-=(const RConstIterator it){
            this->value_ptr_ += it.value_ptr_;
            return *this;
        }

        /**
         * rations operators
         */

        bool operator<(const RConstIterator &rhs) const {
            return this->value_ptr_ > rhs.value_ptr_;

        }

        bool operator>(const RConstIterator &rhs) const {
            return this->value_ptr_ < rhs.value_ptr_;
        }

        bool operator<=(const RConstIterator &rhs) const {
            return this->value_ptr_ >= rhs.value_ptr_;
        }

        bool operator>=(const RConstIterator &rhs) const {
            return this->value_ptr_ <= rhs.value_ptr_;
        }


    };
};


