#ifndef __CB_CHILD_PIDS_H__
#define __CB_CHILD_PIDS_H__

#include <memory>
#include <vector>
#include "settings.h"

class DLLIMPORT cbChildPIDs
{
    public:
        cbChildPIDs();
        ~cbChildPIDs();

        /** Return a vector full with the PIDs of the child processes of the parent.
          * @param[out] children List of PIDs of the child processes of the parent
          * @param[in] parent PID of the parent process
          */
        void GetChildrenPIDs(std::vector<int> &children, int parent);
    private:
        struct Data;
        std::unique_ptr<Data> m_data;
};

#endif // __CB_CHILD_PIDS_H__

