//
//  CFileOperation.cpp
//  netcat
//
//  Created by Tony Liu on 9/5/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "CFileOperation.h"
CFileOperation::CFileOperation()
{
    m_strLocalPath = "./";
    m_strFileName = "";
    m_iFileDescriber = -1;
}
CFileOperation::CFileOperation(std::string strPath, std::string strFileName)
{
    m_strLocalPath = strPath;
    m_strFileName = strFileName;
    if (!CheckPath(m_strLocalPath))
    {
        fprintf(stdout, "ERROR: CFileOpeartion path check %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(m_strLocalPath.c_str());
        exit(EXIT_FAILURE);
    }
    m_iFileDescriber = -1;
}
CFileOperation::CFileOperation(std::string strPath)
{
    m_strLocalPath = strPath.substr(0, strPath.find_last_of("/"));
    m_strFileName = strPath.substr(strPath.find_last_of("/"), strPath.length() - strPath.find_last_of("/"));
    if (!CheckPath(m_strLocalPath))
    {
        fprintf(stdout, "ERROR: CFileOpeartion path check %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(m_strLocalPath.c_str());
        exit(EXIT_FAILURE);
    }
    m_iFileDescriber = -1;
}
CFileOperation::~CFileOperation()
{
    if (-1 != m_iFileDescriber)
    {
        if (-1 == close(m_iFileDescriber))
        {
            fprintf(stderr, "ERROR: CFileOperation file close %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
            perror(m_strFileName.c_str());
        }
        m_iFileDescriber = -1;
    }
}
//bool CFileOperation::InitMutex()
//{
//    if (0 == pthread_mutex_init(&m_Mutex,NULL))
//    {
//        fprintf(stdout, "ERROR: CFileOpeartion mutex init %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
//        return true;
//    }
//    else
//    {
//        fprintf(stderr, "ERROR: CFileOperation mutex init failed %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
//        perror("mutex_init");
//    }
//    return false;
//}
int CFileOperation::ReadFile(std::string strFileName, void * Buffer, int iOffset, unsigned uiBufferLength)
{
    m_strFileName = strFileName;
    return ReadFile(Buffer, iOffset, uiBufferLength);
}
int CFileOperation::ReadFile(void * Buffer, int iOffset, unsigned uiBufferLength)
{
    int ireturn = -1;
    if (0 > m_iFileDescriber)
    {
        char cfilename[1024] = "\0";
        snprintf(cfilename, sizeof(cfilename), "%s%s%s", m_strLocalPath.c_str(), "/", m_strFileName.c_str());
        m_iFileDescriber = open(cfilename, O_RDONLY, 0600);
        if (0 > m_iFileDescriber)
        {
            fprintf(stderr, "ERROR: CFileOperation file open %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
            perror(cfilename);
        }
        else
        {
            off_t ioffset = lseek(m_iFileDescriber, iOffset, SEEK_CUR);
            if (-1 == ioffset)
            {
                fprintf(stderr, "ERROR: CFileOperation file seek %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
                perror(m_strFileName.c_str());
            }
        }
    }
    ireturn = static_cast<int>(read(m_iFileDescriber, Buffer, uiBufferLength));
    if (0 > ireturn)
    {
        fprintf(stderr, "ERROR: CFileOperation file read %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(m_strFileName.c_str());
    }
    if (-1 == close(m_iFileDescriber))
    {
        fprintf(stderr, "ERROR: CFileOperation file close %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(m_strFileName.c_str());
    }
    m_iFileDescriber = -1;
    return ireturn;
}

bool CFileOperation::WriteFile(void * Buffer, unsigned uiBufferLength)
{
    bool breturn = true;
    if (0 > m_iFileDescriber)
    {
        char cfilename[1024] = "\0";
        snprintf(cfilename, sizeof(cfilename), "%s%s%s", m_strLocalPath.c_str(),"/", m_strFileName.c_str());
        m_iFileDescriber = open(cfilename, O_WRONLY|O_CREAT, 0600);
        //fprintf(stderr, "!!!%s,%s,%d,cfilename=%s\n", __FILE__,__PRETTY_FUNCTION__,__LINE__,cfilename);
        if (0 > m_iFileDescriber)
        {
            fprintf(stderr, "ERROR: CFileOperation file open %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
            perror(m_strFileName.c_str());
            breturn = false;
            return breturn;
        }
    }
    int ireturn = static_cast<int>(write(m_iFileDescriber, Buffer,uiBufferLength));
    if (0 > ireturn)
    {
        fprintf(stderr, "ERROR: CFileOperation file write %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(m_strFileName.c_str());
        breturn = false;
    }
    return breturn;
}
//bool CFileOperation::OpenFile()
//{
//    bool breturn = true;
//    if (-1 == m_iFileDescriber)
//    {
//        char cfilename[1024] = "\0";
//        snprintf(cfilename, sizeof(cfilename), "%s%s%s", m_strLocalPath.c_str(), "\\", m_strFileName.c_str());
//        int ifd = open(cfilename, O_WRONLY|O_CREAT, 0600);
//        if (0 > ifd)
//        {
//            fprintf(stderr, "ERROR: CFileOperation file open %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
//            perror(cfilename);
//            breturn = false;
//        }
//    }
//    return breturn;
//}
bool CFileOperation::CheckPath(std::string &strPath)
{
    bool bret = true;
    
    if ("\0" == strPath || "/" == strPath)
    {
        bret = false;
        return bret;
    }
    if (strPath.find_last_of('/') != strPath.length() - 1)
    {
        strPath += "/";
    }
    std::string strStreamStorePath = strPath;
    std::vector<std::string> vec_subpath;
    vec_subpath.clear();
    
    for (std::string::size_type i = 0;i != strStreamStorePath.length();)
    {
        std::string::size_type findpos = strStreamStorePath.find("/");
        if (findpos != std::string::npos)
        {
            vec_subpath.push_back(strStreamStorePath.substr(i,findpos-i));
            strStreamStorePath = strStreamStorePath.substr(findpos+1,strStreamStorePath.length()-findpos-1);
            i = 0;
        }
        else
        {
            vec_subpath.push_back(strStreamStorePath.substr(i,strStreamStorePath.length()-i));
            break;
        }
    }
    
    std::string::size_type sizetype = vec_subpath.size();
    std::vector<std::string> vec_path;
    vec_path.clear();
    
    std::string strtmppath = "\0";
    for (std::string::size_type i = 1; i != sizetype; ++i)
    {
        strtmppath = strtmppath + "/" + vec_subpath[i];
        vec_path.push_back(strtmppath);
    }
    
    std::vector<std::string>::iterator iter;
    for (iter = vec_path.begin(); iter != vec_path.end(); ++iter)
    {
        if (!CreatePath(*iter))
        {
            bret = false;
            fprintf(stderr, "CFileOperation path %s create failed %s,%s,%d", (*iter).c_str(), __FILE__, __PRETTY_FUNCTION__, __LINE__);
            return bret;
        }
    }
    return bret;
}
bool CFileOperation::CreatePath(const std::string strPath)const
{
    if ("\0" == strPath)
        return false;
    DIR*dir;
    if (NULL == (dir = opendir(strPath.c_str())))
    {
        if (-1 != mkdir(strPath.c_str(),0777))
            return true;
        else
        {
            perror(strPath.c_str());
            return false;
        }
    }
    else
    {
        closedir(dir);
        return true;
    }
}

