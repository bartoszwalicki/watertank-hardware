set(COMPONENT_ADD_INCLUDEDIRS .)
set(COMPONENT_SRCS "ssc-http.c")
set(COMPONENT_REQUIRES "esp-tls" "esp_http_client")
register_component()
