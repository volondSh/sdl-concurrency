set(CPM_DOWNLOAD_VERSION 0.40.7)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
set(CPM_DOWNLOAD_TEMP_LOCATION "${CPM_DOWNLOAD_LOCATION}.tmp")
set(CPM_DOWNLOAD_URLS
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
    "https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/v${CPM_DOWNLOAD_VERSION}/cmake/CPM.cmake"
    "https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/master/cmake/CPM.cmake"
)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cmake")

if(NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    set(CPM_DOWNLOAD_SUCCESS FALSE)
    set(CPM_DOWNLOAD_ERROR_DETAILS "")

    foreach(CPM_DOWNLOAD_URL IN LISTS CPM_DOWNLOAD_URLS)
        file(REMOVE "${CPM_DOWNLOAD_TEMP_LOCATION}")
        file(DOWNLOAD
            "${CPM_DOWNLOAD_URL}"
            "${CPM_DOWNLOAD_TEMP_LOCATION}"
            STATUS CPM_DOWNLOAD_STATUS
            LOG CPM_DOWNLOAD_LOG
            # TLS_VERIFY ON
        )

        list(GET CPM_DOWNLOAD_STATUS 0 CPM_DOWNLOAD_STATUS_CODE)
        list(GET CPM_DOWNLOAD_STATUS 1 CPM_DOWNLOAD_STATUS_TEXT)

        if(CPM_DOWNLOAD_STATUS_CODE EQUAL 0 AND EXISTS "${CPM_DOWNLOAD_TEMP_LOCATION}")
            file(READ "${CPM_DOWNLOAD_TEMP_LOCATION}" CPM_DOWNLOAD_CONTENT)
            string(FIND "${CPM_DOWNLOAD_CONTENT}" "CPMAddPackage" CPM_SIGNATURE_POS)
            if(CPM_SIGNATURE_POS GREATER -1)
                file(RENAME "${CPM_DOWNLOAD_TEMP_LOCATION}" "${CPM_DOWNLOAD_LOCATION}")
                set(CPM_DOWNLOAD_SUCCESS TRUE)
                message(STATUS "CPM.cmake downloaded successfully from ${CPM_DOWNLOAD_URL}")
                break()
            endif()
        endif()

        string(APPEND CPM_DOWNLOAD_ERROR_DETAILS
            "\n- ${CPM_DOWNLOAD_URL}\n  code=${CPM_DOWNLOAD_STATUS_CODE}, text=${CPM_DOWNLOAD_STATUS_TEXT}\n"
        )
    endforeach()

    if(NOT CPM_DOWNLOAD_SUCCESS)
        message(FATAL_ERROR
            "Failed to download a valid CPM.cmake. Tried URLs:${CPM_DOWNLOAD_ERROR_DETAILS}\n"
            "Please check network/proxy settings and GitHub access."
        )
    endif()
endif()

include("${CPM_DOWNLOAD_LOCATION}")

if(NOT COMMAND CPMAddPackage)
    message(FATAL_ERROR
        "CPM.cmake was included but CPMAddPackage is unavailable. "
        "Downloaded file may be incomplete: ${CPM_DOWNLOAD_LOCATION}"
    )
endif()
