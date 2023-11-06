#pragma once

#include <stdexcept>
#include <windows.h>
#include <wininet.h>
#include <string>
#include <vector>

class InternetUtils
{
    HANDLE    session;
    HINTERNET internet;

public:

    class Request
    {
        HANDLE handle;

    public:
        /*******************************************************/
        std::string
        GetString ()
        {
            std::vector<uint8_t> output;
            GetResponse (output);

            return std::string (output.begin (), output.end ()).c_str ();
        }

        /*******************************************************/
        bool
        GetResponse (std::vector<uint8_t> &out)
        {
            DWORD dwSize = 16000;
            DWORD dwDownloaded;

            auto lpszData = new TCHAR[dwSize + 1];

            for (;;)
                {
                    if (!InternetReadFile (handle, lpszData, dwSize,
                                           &dwDownloaded))
                        break;

                    if (dwDownloaded == 0)
                        break;

                    out.insert (out.end (), lpszData, lpszData + dwDownloaded);
                }

            delete[] lpszData;
            
            return true;
        }

        Request (HANDLE handle) : handle (handle) {}
        ~Request () { InternetCloseHandle (handle); }
    };

    void
    Open (const std::string &domain)
    {
        internet = InternetOpen ("123robot", INTERNET_OPEN_TYPE_PRECONFIG, NULL,
                                 NULL, 0);

        if (!internet)
            throw std::runtime_error (
                "Failed to connect to the internet!. Error Code: "
                + std::to_string (GetLastError ()));

        session = InternetConnect (internet, domain.c_str (),
                                   INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                                   INTERNET_SERVICE_HTTP, 0, 0);

        if (!session)
            throw std::runtime_error (
                "Failed to connect to the DWOM website!. Error Code: "
                + std::to_string (GetLastError ()));
    }

    void Close ()
    {
        CloseHandle (session);
        CloseHandle (internet);
    }
    
    /*******************************************************/
    Request
    Get (const std::string &file)
    {
        HANDLE request = HttpOpenRequest (session, "GET", file.c_str (), NULL,
                                          NULL, NULL, INTERNET_FLAG_SECURE, 0);
        HttpSendRequest (request, NULL, 0, NULL, 0);

        return request;
    }

    /*******************************************************/
    Request
    Post (const std::string &url, const std::string &headers,
          const std::string &data)
    {
        HANDLE request = HttpOpenRequest (session, "POST", url.c_str (), NULL,
                                          NULL, NULL, INTERNET_FLAG_SECURE, 0);
        HttpSendRequest (request, headers.c_str (), headers.length (),
                         (char *) data.c_str (), data.length ());

        return request;
    }
};
