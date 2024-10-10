#ifndef _GRETEL_H_INCLUDED_
#define _GRETEL_H_INCLUDED_

struct gretel_struct {
    uint64_t a,b,c,d;
};
typedef  struct gretel_struct  gretel_t;

#define PR_GRETEL_REQUEST_SET 9998
#define PR_GRETEL_RESPONSE_SET 9999
#define GRETEL_A_USER 5

#define GRETEL_HTTP_HEADER "gretel"

gretel_t mkgretel(uint64_t a, uint64_t b, uint64_t c, uint64_t d);

void gretel_set_req(uint64_t a, uint64_t b, uint64_t c, uint64_t d);
void gretel_set_resp(uint64_t a, uint64_t b, uint64_t c, uint64_t d);

void gretel_setg_req(gretel_t grtl);
void gretel_setg_resp(gretel_t grtl);

void gretel_link(ngx_log_t *log, gretel_t prev, gretel_t next);

extern ngx_uint_t gretel_counter;
extern ngx_uint_t GRETEL_PID;


gretel_t gretel_random();

#define GRETEL_DO_STRINGIFY(s) #s
#define GRETEL_STRINGIFY(s) GRETEL_DO_STRINGIFY(s)  /* need extra indirection to force __LINE__ to expand */
#define GRETEL_FILENAME() __FILE__
#define GRETEL_LINENO() GRETEL_STRINGIFY(__LINE__)

void gretel_do_node(ngx_log_t *log, gretel_t node, const char *filename, const char *lineno);
#define gretel_node(log, node) gretel_do_node(log, node, GRETEL_FILENAME(), GRETEL_LINENO())


void gretel_do_bump(ngx_log_t *log, gretel_t cur, gretel_t *req, gretel_t *resp, const char *filename, const char *lineno);
#define gretel_bump(log, cur, req, resp) gretel_do_bump(log, cur, req, resp, GRETEL_FILENAME(), GRETEL_LINENO())

void gretel_format(gretel_t gretel, u_char *buf);

#endif /*_GRETEL_H_INCLUDED_*/
