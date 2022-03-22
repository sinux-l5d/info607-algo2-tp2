#include "quicksort.h"

void swap(Point *a, Point *b)
{
    Point tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

int cmp(Point origine, Point a, Point b)
{
    return Point_orientation(origine, a, b) > 0;
}

void quicksort_with_origin(TabPoints *tab, int first, int last)
{
    int pivot, i, j;
    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;
        while (i < j)
        {
            while (cmp(tab->points[0], tab->points[i], tab->points[pivot]) && i < last)
                i++;
            while (!cmp(tab->points[0], tab->points[j], tab->points[pivot]) && j > first)
                j--;
            if (i < j)
            {
                swap(&tab->points[i], &tab->points[j]);
            }
        }
        swap(&tab->points[pivot], &tab->points[j]);
        quicksort_with_origin(tab, first, j - 1);
        quicksort_with_origin(tab, j + 1, last);
    }
}

void quicksort(TabPoints *tab)
{
    // t[0] est le point le plus bas
    Point *pMin = TabPoints_min(tab);
    swap(pMin, &tab->points[0]);
    quicksort_with_origin(tab, 1, tab->nb - 1);
}