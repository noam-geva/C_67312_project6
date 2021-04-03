#ifndef START_CAP
#include "vl_vector.h"
#endif

template <size_t StaticCapacity = START_CAP>
class VLString : public VLVector<char,StaticCapacity>{


    public:
    typedef char* Iterator;
    typedef const char* ConstIterator;
    typedef std::reverse_iterator<Iterator> RIterator;
    typedef std::reverse_iterator<ConstIterator> RConstIterator;

    /**
     * default constructor
     * using single_value Ctor for VLVector
     */
    VLString(): VLVector<char,StaticCapacity>(1,'\0') {}

    /**
     * implicit CTOR
     * @param string
     */
    explicit VLString(const char* string): VLVector<char,StaticCapacity>(1, '\0') {
        size_t ix = 0;
        while (string[ix]){
            this->PushBack(string[ix]);
            ix++;
        }
    }

    /**
     * Copy CTOR
     * @param rhs
     */
    VLString(const VLString<StaticCapacity> &rhs): VLVector<char,StaticCapacity>(rhs.begin(), rhs.end()+1){}//+1 to enclude \0

    /**
     * return the size without \0
     * @return
     */
    size_t Size() const noexcept override{
        return this->size_ - 1;
    }

    /**
     * @return if size (without \0) is 0
     */
    bool Empty() const noexcept override{
        return this->Size() == 0;
    }

    /**
    * checks index validity.
    * @param index
    * @return data_[index]
    */
    char At(size_t index) const override{
        if (index >= this->Size() || index < 0){
            throw std::out_of_range (BAD_INDEX);
        }
        return this->data_[index];
    }

    /**
     * adds an item at the end of the str but before \0
     *
     * @param char to add at the end.
     */
    void PushBack(const char value) noexcept override{
        this->ResizeUp(1);
        this->data_[this->Size()] = value;
        this->data_[this->size_++] = '\0';
    }

    /**
    *removes the last char from the string resize if necessary.
    */
    void PopBack() noexcept override{
        if (this->Size() == 0) {return;}
        this->ResizeDown(this->size_ - 1);
        (*this)[this->Size()] = '\0';
        this->size_--;
    }

    /**
     * turns the string into an empty string.
     */
     void Clear() noexcept override {
        if (this->is_alloc_){
            delete[] this->data_;
            this->is_alloc_= false;
            this->cap_ = StaticCapacity;
            this->data_ = &this->static_data_[0];
        }
        this->size_ = 1;
        this->static_data_[0] = '\0';

    }

    VLString operator+(const VLString &rhs){
        VLString ret(*this);
        this->cap_ = this->CalculateCapC(rhs.Size());
        for (size_t ix = 0 ; ix < rhs.Size() ; ix++){
            ret.PushBack(rhs.data_[ix]);
        }
        //this->end is iter to size +1 an we want save \0
        //rhs.end() because we DONT want its \0
        return ret;
    }
    VLString operator+(const char &rhs){
        VLString ret(*this);
        ret.PushBack(rhs);
        return ret;
    }

    VLString operator+(const char* rhs){
        VLString ret(*this);
        int ix = 0;
        size_t original_size = ret.size_;
        while(rhs[ix]){
            ret.PushBack(rhs[ix++]);
        }
        if (original_size + ix > StaticCapacity){//ix -1 is the num of element we added
            ret.cap_ = floor(REFACTOR_RATIO * ( (double) original_size + ix));
            ret.data_ = new char[ret.cap_];
        }
        return ret;
    }

    VLString operator+=(const VLString &rhs){
         *this = *this + rhs;
         return *this;
    }

    VLString &operator+=(const char &rhs){
        this->PushBack(rhs);
         return *this;
    }

    VLString operator+=(const char* rhs){
        *this = *this + rhs;
        return *this;
    }


    /**
     * we must override these iterators so we will not return \0
     */

    Iterator end() const noexcept override{
        return &this->data_[this->Size()];
    }

    ConstIterator cend() const noexcept override{
        return &this->data_[this->Size()];
    }


    RIterator rbegin() const noexcept override{
        return std::reverse_iterator<Iterator>(&this->data_[this->Size()]);
    }

    RConstIterator crbegin() const noexcept override{
        return std::reverse_iterator<Iterator>(&this->data_[this->Size()]);
    }

};


