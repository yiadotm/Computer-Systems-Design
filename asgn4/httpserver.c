#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>
#include <err.h>

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "asgn2_helper_funcs.h"
#include "queue.h"
#include "request.h"
#include "response.h"
#include "debug.h"
#include "rwlock.h"
#include "connection.h"
#include "arg.h"
#include "file_lock.h"

#define UNUSED(x) ((void) (x))
#define OPTIONS   "t:"

void handle_connection(int connfd, FileLock *map, Arguments *a);
void audit(conn_t *conn, const Response_t *res, char *uri, char *requestID, int com);
void handle_get(conn_t *, FileLock *map, Arguments *a);
void handle_put(conn_t *, FileLock *map, Arguments *a);
void handle_unsupported(conn_t *);
// void *dispatcherThread(void *arg);
void *workerThreads(void *arg);

// void *dispatcherThread(void *arg) {
//     Arguments *a = (Arguments *) arg;

//     while (1) {
//         int con_fd = listener_accept(a->sock);
//         // if (con_fd == -1) {
//         //     printf("error: listener_accept returned -1\n%s\n", strerror(errno));
//         // }
//         queue_push(a->q, (void *) &con_fd);
//         // printf("success: %d,fd: %d\n", b, con_fd);
//     }
//     return arg;
// }

void *workerThreads(void *arg) {
    Arguments *a = (Arguments *) arg;
    queue_t *q = a->q;
    while (1) {
        // void *elem = NULL;
        // int b = queue_pop(q, &elem);
        // if (b) {
        uintptr_t fd = 0;
        queue_pop(q, (void **) &fd);
        // printf("fd: %d, success: %d\n", fd, b);
        handle_connection((int) fd, a->fi, a);
        // close(fd);

        // }
    }
    return arg;
}

int main(int argc, char *argv[]) {
    //chatGPT
    int threads = 1; // Default number of threads
    size_t port = 0;
    // Process command-line options
    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            threads = atoi(optarg); // Convert the argument to an integer
            break;
        case '?':
            // Invalid option or missing argument
            fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
            return 1;
        }
    }

    // Process the remaining non-option arguments (the port)
    if (optind < argc) {
        char *endptr = NULL;
        port = (size_t) strtoull(argv[optind], &endptr, 10);
        if (endptr && *endptr != '\0') {
            warnx("invalid port number: %s", argv[1]);
            return EXIT_FAILURE;
        }
        // Now you can use 'threads' and 'port' in your program
        // printf("Threads: %d\n", threads);
        // printf("Port: %zu\n", port);
    } else {
        // Missing required arguments
        fprintf(stderr, "Usage: %s [-t threads] <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //initialize pthreads
    // pthread_t dispatcherThreadId,
    pthread_t workerThreadIds[threads];
    queue_t *queue = queue_new(4096);
    FileLock *file = file_lock_new(4096);
    Arguments *a = newArguments(queue, file);

    // uintptr_t workers[threads];

    // // Create dispatcher thread
    // if (pthread_create(&dispatcherThreadId, NULL, dispatcherThread, (void *) a) != 0) {
    //     perror("Error creating dispatcher thread");
    //     exit(EXIT_FAILURE);
    // }

    //create worker threads
    for (int i = 0; i < threads; i++) {
        if (pthread_create(&workerThreadIds[i], NULL, workerThreads, (void *) a) != 0) {
            perror("Error creating worker thread");
            exit(EXIT_FAILURE);
        }
    }
    signal(SIGPIPE, SIG_IGN);
    //check if httpserver can bind to the provided port
    Listener_Socket sock;
    int listen_fd = listener_init(&sock, port);
    if (listen_fd == -1) {
        fprintf(stderr, "listener_init failed\n");
        return 1;
    }
    while (1) {
        // int con_fd = listener_accept(&sock);
        // if (con_fd == -1) {
        //     printf("error: listener_accept returned -1\n%s\n", strerror(errno));
        // }
        // queue_push(a->q, (void *) &con_fd);
        // printf("success: %d,fd: %d\n", b, con_fd);

        intptr_t fd = listener_accept(&sock);
        queue_push(a->q, (void *) fd);
    }
    // Join the dispatcher thread
    // pthread_join(dispatcherThreadId, NULL);
    // Join the worker threads
    // for (int i = 0; i < threads; ++i) {
    //     pthread_join(workerThreadIds[i], NULL);
    // }

    // while (1) {
    //     int con_fd = listener_accept(&sock);
    //     handle_connection(con_fd);
    //     close(con_fd);
    // }

    //free stuff
    queue_delete(&queue);
    freeArguments(&a);
    file_lock_delete(&file);
    return EXIT_SUCCESS;
}

void handle_connection(int connfd, FileLock *map, Arguments *a) {

    conn_t *conn = conn_new(connfd);

    const Response_t *res = conn_parse(conn);

    if (res != NULL) {
        conn_send_response(conn, res);
    } else {
        // debug("%s", conn_str(conn));
        const Request_t *req = conn_get_request(conn);
        if (req == &REQUEST_GET) {
            // printf("here1\n");
            handle_get(conn, map, a);
            // printf("here2\n");
        } else if (req == &REQUEST_PUT) {
            handle_put(conn, map, a);
        } else {
            handle_unsupported(conn);
        }
    }
    close(connfd);
    conn_delete(&conn);
}
void audit(conn_t *conn, const Response_t *res, char *uri, char *requestID, int com) {
    const char *command = (com == 0) ? "GET" : "PUT";

    if (res) {
        conn_send_response(conn, res);
        fprintf(stderr, "%s,/%s,%d,%s\n", command, uri, response_get_code(res), requestID);

    } else {
        if (com == 0) {
            fprintf(stderr, "%s,/%s,%d,%s\n", command, uri, 200, requestID);
        }
    }
}
//replace out, use return after copy out to goto out
void handle_get(conn_t *conn, FileLock *map, Arguments *a) {
    // UNUSED(a);
    char *uri = conn_get_uri(conn);
    char *requestID = conn_get_header(conn, "Request-Id");
    const Response_t *res = NULL;
    // debug("handling get request for %s", uri);
    // What are the steps in here?
    bool existed = access(uri, F_OK) == 0;
    // int unlock = 0;
    // 1. Open the file.
    // fprintf(stderr, "get lock, uri: %s, requestID: %s\n", uri, requestID);
    pthread_mutex_lock(&a->mutex);
    // fprintf(stderr, "get file lock, uri: %s, requestID: %s\n", uri, requestID);

    file_lock_read_lock(map, uri);
    int fd = open(uri, O_RDONLY);
    if (!existed) {
        res = &RESPONSE_NOT_FOUND;
        audit(conn, res, uri, requestID, 0);
        // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

        file_lock_read_unlock(map, uri);
        // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

        pthread_mutex_unlock(&a->mutex);

        return;
    }

    // If  open it returns < 0, then use the result appropriately
    if (fd < 0) {
        // debug("%s: %d", uri, errno);
        // unlock = 1;
        //   a. Cannot access -- use RESPONSE_FORBIDDEN
        if (errno == EACCES || errno == EISDIR) {
            res = &RESPONSE_FORBIDDEN;
            audit(conn, res, uri, requestID, 0);
            // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

            file_lock_read_unlock(map, uri);
            // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

            pthread_mutex_unlock(&a->mutex);

            return;
        }
        //   b. Cannot find the file -- use RESPONSE_NOT_FOUND
        else if (errno == ENOENT) {
            res = &RESPONSE_NOT_FOUND;
            audit(conn, res, uri, requestID, 0);
            // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

            file_lock_read_unlock(map, uri);
            // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

            pthread_mutex_unlock(&a->mutex);

            return;
        }
        //   c. other error? -- use RESPONSE_INTERNAL_SERVER_ERROR
        // (hint: check errno for these cases)!
        else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            audit(conn, res, uri, requestID, 0);
            // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

            file_lock_read_unlock(map, uri);
            // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

            pthread_mutex_unlock(&a->mutex);

            return;
        }
    }
    // 2. Get the size of the file.
    // (hint: checkout the function fstat)!

    //make rwlock and put into map
    // pthread_mutex_lock(&a->mutex);

    // file_lock_read_lock(map, uri);
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1) {
        audit(conn, res, uri, requestID, 0);
        // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

        file_lock_read_unlock(map, uri);
        close(fd);
        // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

        pthread_mutex_unlock(&a->mutex);

        return;
    }
    // Get the size of the file.
    uint64_t size = fileInfo.st_size;
    // 3. Check if the file is a directory, because directories *will*
    // open, but are not valid.
    // (hint: checkout the macro "S_IFDIR", which you can use after you call fstat!)
    if (fileInfo.st_mode & S_IFDIR) {
        res = &RESPONSE_FORBIDDEN;
        audit(conn, res, uri, requestID, 0);
        // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

        file_lock_read_unlock(map, uri);
        close(fd);
        // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

        pthread_mutex_unlock(&a->mutex);

        return;
    }
    // uint16_t code = response_get_code(res);
    // fprintf(stderr, "GET,/%s,%" PRIu16 "\n", uri, code);

    // 4. Send the file
    // (hint: checkout the conn_send_file function!)
    //lock

    res = conn_send_file(conn, fd, size);
    audit(conn, res, uri, requestID, 0);
    // fprintf(stderr, "get file unlock, uri: %s, requestID: %s\n", uri, requestID);

    file_lock_read_unlock(map, uri);
    close(fd);
    //unlock
    // fprintf(stderr, "get unlock, uri: %s, requestID: %s\n", uri, requestID);

    pthread_mutex_unlock(&a->mutex);
}

void handle_unsupported(conn_t *conn) {
    // debug("handling unsupported request");
    // char *uri = conn_get_uri(conn);
    // char *requestID = conn_get_header(conn, "Request-Id");

    // send responses
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
    // fprintf(stderr, "UNSUPPORTED,%s,%d,%s\n", uri, response_get_code(&RESPONSE_NOT_IMPLEMENTED),
    //     requestID);
}

void handle_put(conn_t *conn, FileLock *map, Arguments *a) {
    // UNUSED(a);
    char *uri = conn_get_uri(conn);

    char *requestID = conn_get_header(conn, "Request-Id");
    const Response_t *res = NULL;
    // debug("handling put request for %s", uri);

    // Check if file already exists before opening it.
    bool existed = access(uri, F_OK) == 0;
    // debug("%s existed? %d", uri, existed);
    // fprintf(stderr, "put lock, uri: %s, requestID: %s\n", uri, requestID);

    pthread_mutex_lock(&a->mutex);
    // fprintf(stderr, "put file lock, uri: %s, requestID: %s\n", uri, requestID);

    file_lock_write_lock(map, uri);
    // Open the file..
    int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    if (fd < 0) {
        debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
            conn_send_response(conn, res);
            fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), requestID);
            // pthread_mutex_unlock(&a->mutex);
            // fprintf(stderr, "put file unlock, uri: %s, requestID: %s\n", uri, requestID);

            file_lock_write_unlock(map, uri);
            // fprintf(stderr, "put unlock, uri: %s, requestID: %s\n", uri, requestID);

            pthread_mutex_unlock(&a->mutex);

            return;

        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            conn_send_response(conn, res);
            fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), requestID);
            // pthread_mutex_unlock(&a->mutex);
            // fprintf(stderr, "put file unlock, uri: %s, requestID: %s\n", uri, requestID);

            file_lock_write_unlock(map, uri);
            // fprintf(stderr, "put unlock, uri: %s, requestID: %s\n", uri, requestID);

            pthread_mutex_unlock(&a->mutex);

            return;
        }
    }
    // file_lock_write_unlock(map, uri);

    // ftruncate(fd, 0);
    pthread_mutex_unlock(&a->mutex);

    res = conn_recv_file(conn, fd);

    if (res == NULL && existed) {
        res = &RESPONSE_OK;
    } else if (res == NULL && !existed) {
        res = &RESPONSE_CREATED;
    }
    // fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), requestID);

    // out:
    conn_send_response(conn, res);
    // audit(conn, res, uri, requestID, 1);

    fprintf(stderr, "PUT,/%s,%d,%s\n", uri, response_get_code(res), requestID);
    // fprintf(stderr, "put file unlock, uri: %s, requestID: %s\n", uri, requestID);

    file_lock_write_unlock(map, uri);
    close(fd);
    // fprintf(stderr, "put unlock, uri: %s, requestID: %s\n", uri, requestID);

    // pthread_mutex_unlock(&a->mutex);
}

//questions: problem with deadlock or
//[Errno 107] Transport endpoint is not connected
//tips on where to find it
//think its mainly in the get function or in FileLock
//could be with readerlock/writerlock

//check if locking correct spot
