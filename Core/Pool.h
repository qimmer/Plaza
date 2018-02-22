#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>

template<typename T>
class Pool
{
private:
    struct Entry
    {
        Entry() : is_occupied(false) {}

        bool is_occupied;
        T data;
    };

    Vector<Entry> entries;
    Vector<size_t> free_indices;
public:
    T& operator[] (size_t index);
    const T& operator[] (size_t index) const;
    bool IsValid(size_t index) const;
    size_t End() const;

    size_t Add();
    bool Insert(size_t index);
    bool Remove(size_t index);
};

template<typename T>
T& Pool<T>::operator[](size_t index)
{
    return this->entries[index].data;
}

template<typename T>
const T& Pool<T>::operator[](size_t index) const
{
    return this->entries[index].data;
}

template<typename T>
bool Pool<T>::IsValid(size_t index) const
{
    return this->entries.size() > index && this->entries[index].is_occupied;
}

template<typename T>
size_t Pool<T>::End() const
{
    return this->entries.size();
}

template<typename T>
bool Pool<T>::Insert(size_t index)
{
    if(this->entries.size() <= index)
    {
        this->entries.resize(index + 1);
    }

    if(this->entries[index].is_occupied)
    {
        return false;
    }

    this->entries[index].is_occupied = true;

    for(size_t i = 0; i < this->free_indices.size(); ++i) {
        if(this->free_indices[i] == index) {
            this->free_indices.erase(this->free_indices.begin() + i);
            break;
        }
    }

    return true;
}

template<typename T>
bool Pool<T>::Remove(size_t index)
{
    if(this->entries.size() <= index)
    {
        return false;
    }

    if(!this->entries[index].is_occupied)
    {
        return false;
    }

    this->entries[index].is_occupied = false;
    this->entries[index].data = T();
    this->free_indices.push_back(index);
    return true;
}

template<typename T>
size_t Pool<T>::Add() {
    size_t index = this->End();
    if(this->free_indices.size() > 0) {
        index = this->free_indices[this->free_indices.size() - 1];
        this->free_indices.pop_back();
    }

    this->Insert(index);

    return index;
}

#endif