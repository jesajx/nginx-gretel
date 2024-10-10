
#include <ngx_core.h>

gretel_t mkgretel(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    gretel_t res = {};
    res.a = a;
    res.b = b;
    res.c = c;
    res.d = d;
    return res;
}

void gretel_set_req(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    prctl(PR_GRETEL_REQUEST_SET, a, b, c, d);
}
void gretel_set_resp(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    prctl(PR_GRETEL_RESPONSE_SET, a, b, c, d);
}

void gretel_setg_req(gretel_t grtl) {
    gretel_set_req(grtl.a, grtl.b, grtl.c, grtl.d);
}
void gretel_setg_resp(gretel_t grtl) {
    gretel_set_resp(grtl.a, grtl.b, grtl.c, grtl.d);
}


void gretel_do_node(ngx_log_t *log, gretel_t node, const char *filename, const char *lineno) {
    ngx_log_error(NGX_LOG_ALERT, log, 0, "GRETEL NODE %016xl-%016xl-%016xl-%016xl gp=%ul pid=%ul file=%s lineno=%s",
                    node.a,
                    node.b,
                    node.c,
                    node.d,
                    GRETEL_PID,
                    ngx_pid,
                    (u_char*)filename,
                    (u_char*)lineno
                    );
}

void gretel_link(ngx_log_t *log, gretel_t prev, gretel_t next) {
    ngx_log_error(NGX_LOG_ALERT, log, 0, "GRETEL LINK %016xl-%016xl-%016xl-%016xl->%016xl-%016xl-%016xl-%016xl",
                    prev.a,
                    prev.b,
                    prev.c,
                    prev.d,
                    next.a,
                    next.b,
                    next.c,
                    next.d
                    );
}

static uint64_t gretel_random64() {
    return ((ngx_random() << 32) | ngx_random());
}

gretel_t gretel_random() {
    gretel_t res = {};
    res.a = GRETEL_A_USER;
    res.b = gretel_random64();
    res.c = gretel_random64();
    res.d = gretel_random64();
    return res;
}

ngx_uint_t gretel_counter;
ngx_uint_t GRETEL_PID;

void gretel_do_bump(ngx_log_t *log, gretel_t cur, gretel_t *req, gretel_t *resp, const char *filename, const char *lineno) {
    gretel_t old_end = *resp;
    if (old_end.a == 0) {
        if (cur.a == 0) {
            /* new_start -> new_end */
            gretel_t new_start = gretel_random();
            gretel_t new_end = gretel_random();
            *resp = new_end;
            *req = new_start;
            gretel_setg_resp(new_end);
            gretel_setg_req(new_start);

            gretel_do_node(log, new_start, filename, lineno);
            gretel_do_node(log, new_end, filename, lineno);
            //gretel_link(log, new_start, new_end);
        } else {
            /* cur -> new_start -> new_end */
            gretel_t new_start = gretel_random();
            gretel_t new_end = gretel_random();
            *resp = new_end;
            *req = new_start;
            gretel_setg_resp(new_end);
            gretel_setg_req(new_start);

            gretel_do_node(log, new_start, filename, lineno);
            gretel_do_node(log, new_end, filename, lineno);
            gretel_link(log, cur, new_start);
            //gretel_link(log, new_start, new_end);
        }
    } else {
        if (cur.a == 0) {
            /* old_end (=new_start) -> new_end */
            gretel_t new_start = old_end;
            gretel_t new_end = gretel_random();
            *resp = new_end;
            *req = new_start;
            gretel_setg_resp(new_end);
            gretel_setg_req(new_start);

            gretel_do_node(log, new_end, filename, lineno);
            //gretel_link(log, new_start, new_end);
        } else {
            /*
             *     cur -+
             *          |
             *      merge_node -> new_end
             *          |
             * old_end -+
             *
             */
            gretel_t merge_node = gretel_random();
            gretel_t new_end = gretel_random();
            gretel_setg_resp(new_end);
            gretel_setg_req(merge_node);
            *req = merge_node;
            *resp = new_end;


            gretel_do_node(log, merge_node, filename, lineno);
            gretel_do_node(log, new_end, filename, lineno);

            gretel_link(log, old_end, merge_node);
            gretel_link(log, cur, merge_node);
            //gretel_link(log, merge_node, new_end);
        }
    }
}

void gretel_format(gretel_t gretel, u_char *buf) {
    u_char *np = buf + 64;
    uint64_t xs[] = {gretel.d, gretel.c, gretel.b, gretel.a};

    for (ngx_uint_t xi = 0; xi < 4; ++xi) {
        uint64_t x = xs[xi];
        ngx_uint_t num_digs = 0;
        while (x != 0 || num_digs < 16) {
            *--np = "0123456789abcdef"[x % 16];
            x /= 16;
            ++num_digs;
        }
    }
}
