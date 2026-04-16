target_headers(RestLink
    PUBLIC
        global.h config.h debug.h
        apibase.h api.h
        requestinterface.h
        parameter.h parameterlist.h pathparameter.h queryparameter.h
        request.h responsebase.h response.h
        header.h body.h
        abstractrequestinterceptor.h
        abstractrequesthandler.h
    PRIVATE
        apibase_p.h api_p.h
        parameter_p.h pathparameter_p.h queryparameter_p.h header_p.h body_p.h
        request_p.h response_p.h
        abstractrequesthandler_p.h
        emptyresponse_p.h
)

target_sources(RestLink
    PRIVATE
        debug.cpp
        apibase.cpp api.cpp
        requestinterface.cpp
        parameter.cpp pathparameter.cpp queryparameter.cpp
        request.cpp responsebase.cpp response.cpp
        header.cpp body.cpp
        abstractrequestinterceptor.cpp
        abstractrequesthandler.cpp
)
