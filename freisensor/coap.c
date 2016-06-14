#include <FreeRTOS.h>
#include "espressif/esp_common.h"

#include "coap/er-coap-13.h"
//#include "coap/er-coap-13-transactions.h"
#include "coap/uri.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

uint16_t create_serialized_request(char *host, char *path, char *port, char *payload, uint8_t *buf) {
  coap_packet_t *request = (coap_packet_t *)malloc(sizeof(coap_packet_t));

  coap_init_message(request, COAP_TYPE_NON, COAP_POST, 0);
  coap_set_header_uri_path(request, path);
  coap_set_header_uri_host(request, host);
  coap_set_header_content_type(request,APPLICATION_JSON);
  coap_set_payload(request, payload, strlen(payload));
  request->mid = coap_get_mid();
  uint16_t length = coap_serialize_message(request, buf);
  free(request);
  return length;
}

int send_message(char *url, char* payload) {
  struct addrinfo hints;
  struct addrinfo *res;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  coap_uri_t *uri = coap_new_uri((unsigned char*)url, strlen(url)); 
  if(!uri) {
    return -1;
  } 

  char port[6];
  char host[64];
  char path[128];

  sprintf(port, "%d", uri->port);
  sprintf(path, "%s", uri->path.s);
  memcpy(host, uri->host.s, uri->host.length);
  host[uri->host.length] = '\0';

  printf("Sending message to host %s on port %s on path %s\n", host, port, path);

  int err;
  while (1) {
    err = getaddrinfo(host, port, &hints, &res);
    if (err == 0)
        break;
    printf("DNS lookup failed err=%d\r\n", err);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }

  int s = socket(res->ai_family, res->ai_socktype, 0);
  if(s < 0) {
      printf("Can't create local socket\n");
      return -2;
  }

  if(connect(s, res->ai_addr, res->ai_addrlen) != 0){
    printf("Can't connect local socket\n");
    close(s);
    return -3;
  }
  freeaddrinfo(res);
  uint8_t buf[COAP_MAX_PACKET_SIZE+1];
  uint16_t packet_length = create_serialized_request(host, path, port, payload, buf);
  int ret = write(s, buf, packet_length);
  if(ret < 0){
    printf("Can't write to socket. Return code %d\n",ret);
  }
  close(s);
  free(uri);
  return ret;
}
