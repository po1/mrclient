#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include <netdb.h>
#include <arpa/inet.h>      /* inet_ntoa() to format IP address */
#include <netinet/in.h>     /* in_addr structure */

#include <pthread.h>
#include <signal.h>

int sock;
WINDOW *wmain;
int theend = 0;

wchar_t rbuffer[200][1024];
wchar_t *rbuf_[200];
int rbl = 0;

void print_buf(WINDOW *win, wchar_t *buf[], int sp);

void * handle_recv(void * args)
{
	int i, curline = 0;
	char buf[2048];

	for (i = 0; i < 200; i++)
		rbuf_[i] = rbuffer[i];

	while(1)
	{
		int i, j = curline, n;
		wchar_t * pbuf;
		n = recv(sock, buf, 2048, 0);
		buf[n] = 0;
		pbuf = rbuffer[rbl%200];
		if (!n)
			break;
		for (i = 0; i < n; i++)
			if (buf[i] == '\n')
			{
				pbuf[j] = 0;
				j = 0;
				pbuf = rbuffer[++rbl%200];

			} else
			{
				pbuf[j++] = buf[i];
			}

		curline = j;
//		waddstr(wmain, buf);
//		wrefresh(wmain);
		print_buf(wmain, rbuf_, rbl);
	}

	theend = 1;
	endwin();			/* End curses mode		  */
	exit(0);
}

int scur_pos (wchar_t *buf, int cur);

int input_box(WINDOW *win, wchar_t *buf, int cur, int old)
{
	int i, l = wcslen(buf);

	if (cur <  old) // out of screen: left
		if (cur[buf])
			i = cur;
		else
			for (i = l; i > 0 && scur_pos(buf + i, cur - i) < COLS - 1; i--);
	else if (scur_pos(buf + old, cur - old) > COLS - 1) // out of screen: right
		for (i = l; scur_pos(buf + i, cur - i) < COLS - 1; i--);
	else
		i = old;
	
	waddwstr(win, buf + i);

	return i;
}

int scur_pos (wchar_t *buf, int cur)
{
	int i, p = 0;
	for (i = 0; i < cur; i++)
		p += wcwidth(buf[i]);

	return p;
}

WINDOW *tbox;

void print_buf(WINDOW *win, wchar_t *buf[], int sp)
{
	int i, j = 0, or, oc;
	getyx(stdscr, or, oc);
	if (sp > LINES - 2)
		i = sp - LINES + 2;
	else
		i = 0;

	wclear(win);

	for (; i < sp; i++, j++)
		mvwaddwstr(win, j, 0, buf[i%200]);

	wrefresh(wmain);
	move(or, oc);
	wrefresh(tbox);
}

void sigint_handler(int s)
{
	endwin();
	shutdown(sock, SHUT_RDWR);
	close(sock);
	signal(SIGINT, SIG_DFL);
	kill(getpid(), SIGINT);
}

int main(int argc, char *argv[])
{	
	wchar_t scroll_[100][1024];
	wchar_t buf[1024];
	int sp = 0, i;
	wchar_t *scroll[100];
    struct hostent *host;     /* host information */
    struct sockaddr_in server;
	pthread_t thr;

	setlocale(LC_ALL, "");

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		return 1;
	}
	if ((host = gethostbyname(argv[1])) == NULL) 
	{
		fprintf(stderr, "Could not resolve '%s'\n", argv[1]);
		return 1;
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server, 0, sizeof(server));       /* Clear struct */
	server.sin_family = AF_INET;                  /* Internet/IP */
	server.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);  /* IP address */
	server.sin_port = htons(atoi(argv[2]));       /* server port */
	/* Establish connection */
	if (connect(sock,
				(struct sockaddr *) &server,
				sizeof(server)) < 0) 
	{
		fprintf(stderr,"Failed to connect with server\n");
		return 1;
	}

	for (i = 0; i < 100; i++)
		scroll[i] = scroll_[i];

	initscr();			/* Start curses mode 		  */
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();

	wmain = newwin(LINES-2, COLS, 0, 0);
	tbox = newwin(1, COLS, LINES-1, 0);
	mvhline(LINES-2, 0, '_', COLS);
	move(LINES-1, 0);
	wrefresh(tbox);

	pthread_create( &thr, NULL, handle_recv, NULL);

	while (!theend)
	{
		char cbuf[1024];
		int i;
		tbox_gets(tbox, buf, 1024, scroll, 100);
		move(LINES-1, 0);
		memcpy(rbuffer[rbl++], buf, wcslen(buf) * sizeof(wchar_t));
		print_buf(wmain, rbuf_, rbl);
		for (i = 0; i < wcslen(buf); i++)
			cbuf[i] = buf[i];
		cbuf[i] = '\n';
		cbuf[i+1] = 0;

		send(sock, cbuf, strlen(cbuf), 0);
	}

	endwin();			/* End curses mode		  */

	return 0;
}

int tbox_gets (WINDOW *tbox, wchar_t *buf, int size, wchar_t **scroll, int ssize)
{

	int oldc = 0, bp = 0, cur = 0;
	static int sp = 0, spl = 0;
	int curs = sp;
	wint_t ch;

	buf[0] = 0;

	while (1)
	{
		get_wch(&ch);			/* Wait for user input */
		if (bp > size - 1)
			continue;

		if (ch == KEY_BACKSPACE)
		{
			int i;
			if (cur < 1)
				continue;
			for (i = cur; buf[i-1]; i++)
				buf[i-1] = buf[i];
			bp--;
			cur--;
		} else

		if (ch == '\n')
		{
			if (bp < 1)
				continue;

			werase(tbox);
			wrefresh(tbox);
			if (curs != sp)
				sp--;
			memcpy(scroll[sp++ % ssize], buf, (bp + 1) * sizeof(wchar_t));
			if (!sp%ssize)
				spl = 1;

			return bp;

		} else
		if (ch == KEY_LEFT)
		{
			if (cur > 0)
				cur--;
		} else

		if (ch == KEY_RIGHT)
		{
			if (cur < bp)
				cur++;
		} else
		if (ch == KEY_UP)
		{
			int i;

			if (!spl && curs < 1) 
				continue;
			if (spl && curs < sp - ssize + 1)
				continue;
			if (curs == sp)
				memcpy(scroll[sp++ % ssize], buf, (bp + 1) * sizeof(wchar_t));

			curs--;

			for (i = 0; scroll[curs][i]; i++)
				buf[i] = scroll[curs][i];

			buf[i] = 0;
			cur = bp = i;
		} else
		if (ch == KEY_DOWN)
		{
			int i;

			if (curs == sp)
				continue;
			if (curs == sp - 2)
				sp--;

			curs++;

			for (i = 0; scroll[curs][i]; i++)
				buf[i] = scroll[curs][i];

			buf[i] = 0;
			cur = bp = i;
		} else
		if (ch == KEY_HOME)
		{
			cur = 0;
		} else
		if (ch == KEY_END)
		{
			cur = bp;
		} else
		if (ch == KEY_DC)
		{
			int i;
			if (cur > bp - 1)
				continue;
			for (i = cur; buf[i]; i++)
				buf[i] = buf[i+1];
			bp--;
		} else
		{
			int i;
			for (i = bp; i > cur; i--)
				buf[i] = buf[i-1];

			buf[cur++] = ch;
			buf[++bp] = 0;
		}

		werase(tbox);
		oldc = input_box(tbox, buf, cur, oldc);
		wrefresh(tbox);
		move(LINES - 1, scur_pos(buf + oldc, cur - oldc));

	}
}
