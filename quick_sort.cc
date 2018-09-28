#include <iostream>
#include <vector>

using namespace std;

void print_vec(const vector<int> &vec, size_t low=1, size_t high=0)
{
    if (low > high) {
        // print the whole vector
        low = 0;
        high = vec.size()-1;
    }
    for (size_t i = low; i <= high; ++i)
        cout << vec[i] << " ";
    cout << endl;
}

int partition(vector<int> &vec, size_t low, size_t high)
{
    int pivot = vec[high];
    size_t p = low;
    for (size_t idx = low; idx < high; ++idx) {
        if (vec[idx] < pivot)
            swap(vec[p++], vec[idx]);
    }
    swap(vec[p], vec[high]);
    return p;
}

void quick_sort(vector<int> &vec, size_t low, size_t high)
{
    if ((int)high < (int)low + 1)
        return;
    int pivot = partition(vec, low, high);
    quick_sort(vec, low, pivot-1);
    quick_sort(vec, pivot+1, high);
}

int main()
{
    vector<int> nums{4,6,1,21,8,7,0,5};
    //vector<int> nums{9,9,7,6,5,4,3,2,1};
    //vector<int> nums{3,1};
    cout << "Original:   ";
    print_vec(nums);

    quick_sort(nums, 0, nums.size()-1);

    cout << "After sort: ";
    print_vec(nums);

    return 0;
}
