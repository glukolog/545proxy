/* Copyright 37pool.com. All rights reserved.
 *
 * Maintainer:
 *     Huang Le <4tar@37pool.com>
 *
 * The software is published under GNU Affero General Public License version 3,
 * or GNU AGPL v3.  The GNU AGPL is based on the GNU GPL, but has an additional
 * term to allow users who interact with the licensed software over a network to
 * receive the source for that program.  For more information, please refer to:
 *
 *     http://www.gnu.org/licenses/agpl.html
 *     http://www.gnu.org/licenses/why-affero-gpl.html
 *
 * WARN: THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _DEFS_H_
#define _DEFS_H_

#include <assert.h>
#include <stdint.h>
#include <uv.h>
#include "stratum.h"

#define PROXY_VER		1.4

#define CL_N    "\x1B[0m"
#define CL_RED  "\x1B[31m"
#define CL_GRN  "\x1B[32m"
#define CL_YLW  "\x1B[33m"
#define CL_BLU  "\x1B[34m"
#define CL_MAG  "\x1B[35m"
#define CL_CYN  "\x1B[36m"

#define CL_BLK  "\x1B[22;30m" /* black */
#define CL_RD2  "\x1B[22;31m" /* red */
#define CL_GR2  "\x1B[22;32m" /* green */
#define CL_BRW  "\x1B[22;33m" /* brown */
#define CL_BL2  "\x1B[22;34m" /* blue */
#define CL_MA2  "\x1B[22;35m" /* magenta */
#define CL_CY2  "\x1B[22;36m" /* cyan */
#define CL_SIL  "\x1B[22;37m" /* gray */

#ifdef WIN32
#define CL_GRY  "\x1B[01;30m" /* dark gray */
#else
#define CL_GRY  "\x1B[90m"    /* dark gray selectable in putty */
#endif
#define CL_LRD  "\x1B[01;31m" /* light red */
#define CL_LGR  "\x1B[01;32m" /* light green */
#define CL_YL2  "\x1B[01;33m" /* yellow */
#define CL_LBL  "\x1B[01;34m" /* light blue */
#define CL_LMA  "\x1B[01;35m" /* light magenta */
#define CL_LCY  "\x1B[01;36m" /* light cyan */

#define CL_WHT  "\x1B[01;37m" /* white */


struct miner_ctx;

typedef void (*submit_share)( struct miner_ctx *px, const char *miner,
	const char* jobid, const char *xn2, const char *ntime, const char *nonce );

struct pool_ctx;
struct proxy_config;

typedef struct pool_config {
	struct proxy_config *conf;
	char host[64];
	unsigned short port:16;
	int priority:3;
	unsigned int weight:20;
	unsigned int timeout:25;
	char miner[64];
	char passwd[32];
	char cbaddr[36];
	uint64_t cbtotal;
	double cbperc;
	struct pool_ctx *px;
} pool_config;

typedef enum {
	p_disconnected,
	p_connecting,
	p_initializing,
	p_working,
	p_disconnecting,
} pool_status;

typedef struct pool_ctx {
	/*    0h */  char addr[INET6_ADDRSTRLEN];
	/*   30h */  union {
		uv_connect_t conn_req;
		uv_write_t write_req;
	} req;
	/*   f0h */ uint64_t disc_time;
	/*   f8h */ pool_config *conf;
	/*  100h */ uv_loop_t *loop;
	/*  108h */ union {
		uv_handle_t h;
		uv_tcp_t tcp;
		uv_stream_t stream;
	} handle;
	/*  200h */ uv_timer_t timer;
	/*  298h */ stratum_ctx sctx;
	submit_share ss;
	unsigned int count:13;
	unsigned int scount:4;
	unsigned int pos:12;
	unsigned int status:3;
	/*  344h */ char buf[2048], dummy, authtype;
	/*  b46h */ char diff[128], job[1536];
	/* 11c8h */ struct miner_ctx *mx[4096];
} pool_ctx;

typedef struct proxy_config {
	char host[INET6_ADDRSTRLEN];
	char outip[INET6_ADDRSTRLEN];
	unsigned short port, outport;
	unsigned int timeout:28;
	unsigned int count:4;
	uv_loop_t *loop;
	union {
		uv_tcp_t tcp;
		uv_stream_t stream;
	} handle;
	pool_config pools[8];
} proxy_config;

typedef struct miner_ctx {
	/*    0h */ char bind[INET6_ADDRSTRLEN];
	/*   2eh */ char addr[INET6_ADDRSTRLEN];
	/*   5ch */ unsigned short port;
	/*   5eh */ char miner[64];
	/*   7eh */ char agent[64];
	/*   90h */ proxy_config *pxx;
	/*   98h */ pool_ctx *px;
	/*   a0h */ union {
		uv_handle_t h;
		uv_tcp_t tcp;
		uv_stream_t stream;
	} handle;
	/*  198h */ uv_write_t m_req;
	/*  2b8h */ uv_write_t p_req;
	/*  3d8h */ stratum_ctx sctx;
	/*  478h */ char outbuf[2048];
	/*  c78h */ char share[2048];
	/* 1478h */ char buf[2048];
	/* */  char attachInfo[2304];

	unsigned int dummy:12, pos:12, wpos:12;
	unsigned int closing:2, pxreconn:1;
	unsigned int shareLen:12, lastShareLen:12, writeShareLen:12;
} miner_ctx;

/* proxy.c */
int proxy_run();
pool_ctx *pool_pickup( proxy_config *conf );
void attach_miner_to_pool( pool_ctx *px, miner_ctx *mx );
void detach_miner_from_pool( miner_ctx *mx );

/* pool.c */
void pool_connect( pool_ctx *px, struct sockaddr *addr );

/* miner.c */
int miner_clear( miner_ctx *mx );

/* conf.c */
int parse_config( const char* file, proxy_config *conf );

/* util.c */
typedef enum {
	level_debug,
	level_info,
	level_warn,
	level_err,
} log_level;

#if defined(__GNUC__)
# define ATTRIBUTE_FORMAT_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
# define ATTRIBUTE_FORMAT_PRINTF(a, b)
#endif

void pr_debug( const char *fmt, ... ) ATTRIBUTE_FORMAT_PRINTF(1, 2);
void pr_info( const char *fmt, ... ) ATTRIBUTE_FORMAT_PRINTF(1, 2);
void pr_warn( const char *fmt, ... ) ATTRIBUTE_FORMAT_PRINTF(1, 2);
void pr_err( const char *fmt, ... ) ATTRIBUTE_FORMAT_PRINTF(1, 2);
void *xmalloc( unsigned int size );
int hex2bin( unsigned char *b, const char *h, size_t len );
void bin2hex( char *hex, const void *bin, size_t len, int up );
unsigned int varint_decode( const unsigned char *p, size_t size, uint64_t *n );
unsigned int script_to_address(char *out, unsigned int outsz,
	const uint8_t *script, unsigned int scriptsz, int testnet);

#ifndef bswap_16
#define	bswap_16(value) ((((value) & 0xff) << 8) | ((value) >> 8))
#define	bswap_32(value)	\
	(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
	(uint32_t)bswap_16((uint16_t)((value) >> 16)))
#endif 

#ifndef htobe32
#ifndef WORDS_BIGENDIAN
#define htobe32(x) bswap_32(x)
#define be32toh(x) bswap_32(x)
#else
#define htobe32(x) (x)
#define be32toh(x) (x)
#endif
#endif

#ifdef WIN32
#define STIN	static __inline
#define time64	_time64
#else
#define STIN	static inline
#define time64	time
#endif

STIN uint16_t upk_u16le( const void * const buf, const int offset )
{
	const uint8_t * const p = buf;
	return
		(((uint16_t)p[offset])     <<    0) |
		(((uint16_t)p[offset + 1]) <<    8);
}

STIN uint32_t upk_u32le(const void * const buf, const int offset)
{
	const uint8_t * const p = buf;
	return
		(((uint32_t)p[offset])     <<    0) |
		(((uint32_t)p[offset + 1]) <<    8) |
		(((uint32_t)p[offset + 2]) << 0x10) |
		(((uint32_t)p[offset + 3]) << 0x18);
}

STIN uint64_t upk_u64le(const void * const buf, const int offset)
{
	const uint8_t * const p = buf;
	return
		(((uint64_t)p[offset])     <<    0) |
		(((uint64_t)p[offset + 1]) <<    8) |
		(((uint64_t)p[offset + 2]) << 0x10) |
		(((uint64_t)p[offset + 3]) << 0x18) |
		(((uint64_t)p[offset + 4]) << 0x20) |
		(((uint64_t)p[offset + 5]) << 0x28) |
		(((uint64_t)p[offset + 6]) << 0x30) |
		(((uint64_t)p[offset + 7]) << 0x38);
}

#if defined(NDEBUG)
# define ASSERT(exp)
# define CHECK(exp)   do { if (!(exp)) abort(); } while (0)
# define DEBUG_CHECKS (0)
#else
# define ASSERT(exp)  assert(exp)
# define CHECK(exp)   assert(exp)
# define DEBUG_CHECKS (1)
#endif

#define CONTAINER_OF(ptr, type, field) \
  ((type *)((char *)(ptr) - ((char *)&((type *)0)->field)))

#define countof(a) (sizeof(a) / sizeof(a[0]))

#if defined(__GNUC__) && (__GNUC__ > 2)
#define unlikely(expr) (__builtin_expect(!!(expr), 0))
#define likely(expr) (__builtin_expect(!!(expr), 1))
#else
#define unlikely(expr) (expr)
#define likely(expr) (expr)
#endif

#endif  /* DEFS_H_ */
