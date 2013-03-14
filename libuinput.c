/*
 *  Copyright © 2013 Federico Vaga <federico.vaga@gmail.com>
 *  License: GPLv2
 */

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/uinput.h>

#define UINPUT_INTERNAL 1
#include "libuinput.h"

/*
 * uinput_open
 * @path: path to the uinput device. If NULL, the default is /dev/uinput
 *
 * It opens the uinput device.
 * It returns an uinput token to use for all operation on uinput
 */
struct uinput_tkn *uinput_open(char *path) {
	struct uinput_token *tkn;

	tkn = malloc(sizeof(struct uinput_token));
	if (!tkn)
		return NULL ;

	tkn->fd = open(path ? path : "/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (tkn->fd < 0) {
		free(tkn);
		return NULL ;
	}
	return (void *) tkn;
}

/*
 * uinput_close
 * @tkn: the library token created by uinput_open
 *
 * It closes the uinput device
 */
void uinput_close(struct uinput_tkn *tkn) {
	struct uinput_token *t = (void *) tkn;

	if (t->fd >= 0)
		close(t->fd);
}

/*
 * uinput_create_new_device
 * @tkn: the library token created by uinput_open
 * @dev: description of the new input device to create
 *
 * It creates a new uinput device based on the information within the data
 * structure uinput_user_dev. Remember, you must set the list of supported
 * events before create a new device.
 */
int uinput_create_new_device(struct uinput_tkn *tkn,
		struct uinput_user_dev *dev) {
	struct uinput_token *t = (void *) tkn;
	ssize_t len;
	int err;

	if (!tkn || !dev) {
		errno = EINVAL;
		return -1;
	}
	/* Write the configuration of the new input device */
	len = write(t->fd, dev, sizeof(struct uinput_user_dev));
	if (len != sizeof(struct uinput_user_dev)) {
		errno = EIO;
		return -1;
	}
	/* Create the new input device */
	err = ioctl(t->fd, UI_DEV_CREATE);
	if (err == -1) {
		return err;
	}
	/* Device created correctly */
	t->dev = dev;
	return 0;
}

/*
 * uinput_destroy
 * @tkn: the library token created by uinput_open
 *
 * It destroys the device associated to the token
 */
int uinput_destroy(struct uinput_tkn *tkn) {
	struct uinput_token *t = (void *) tkn;
	int err;

	err = ioctl(t->fd, UI_DEV_DESTROY);
	if (err == -1) {
		fprintf(stderr, "Error while destroing device \"%s\"", t->dev->name);
	}
	t->dev = NULL;
	return err;
}

/*
 * uinput_enable_event
 * @tkn: the library token created by uinput_open
 * @type: type of event to enable (read input.h for the list)
 *
 * It enables a type of event
 */
int uinput_enable_event(struct uinput_tkn *tkn, int type) {
	struct uinput_token *t = (void *) tkn;

	return ioctl(t->fd, UI_SET_EVBIT, type);
}

/*
 * uinput_enable_event
 * @tkn: the library token created by uinput_open
 * @type: type of event to set (read input.h for the list)
 * @code: code of the event
 *
 * It sets a valid event handled by the device that you are creating
 */
int uinput_set_valid_event(struct uinput_tkn *tkn, int type, int code) {
	struct uinput_token *t = (void *) tkn;

	return ioctl(t->fd, type, code);
}

/*
 * uinput_set_valid_events
 * @tkn: the library token created by uinput_open
 * @ev: list of events
 * @n: number of events in the list
 *
 * It sets valid events handled by the device that you are creating. It is like
 * uinput_set_valid_event() but it operates on an array of events
 */
int uinput_set_valid_events(struct uinput_tkn *tkn, struct input_event *ev,
		unsigned int n) {
	int i, err = 0;

	for (i = 0; i < n; ++i) {
		err = uinput_set_valid_event(tkn, ev[i].type, ev[i].code);
		if (err)
			break;
	}

	return err;
}

/*
 * uinput_send_event
 * @tkn: the library token created by uinput_open
 * @event: the event to send to device
 *
 * It sends an event to the device
 */
int uinput_send_event(struct uinput_tkn *tkn, struct input_event *ev) {
	struct uinput_token *t = (void *) tkn;

	ssize_t len;

	gettimeofday(&ev->time, 0);
	len = write(t->fd, ev, sizeof(struct input_event));
	if (len != sizeof(struct input_event)) {
		return -1;
	}
	return 0;
}

/*
 * uinput_send_events
 * @tkn: the library token created by uinput_open
 * @event: list of events to send to device
 * @n: number of events in the list
 *
 * It sends a list of events to the device
 */
int uinput_send_events(struct uinput_tkn *tkn, struct input_event *event,
		       unsigned int n, int do_sync) {
	int i, err = 0;
	struct input_event ev = {{0, 0}, EV_SYN, SYN_REPORT, 0};

	for (i = 0; i < n; ++i) {
		err = uinput_send_event(tkn, &event[i]);
		if (err) {
		  fprintf(stderr, "Cannot send event, err %d: %s\n",
			  errno, strerror(errno));
		  break;
		}
	}

	if (!do_sync) /* Skip syncronization */
	    return err;

	/* Do Synchronization */
	err = uinput_send_event(tkn, &ev);
	if (err) {
	    fprintf(stderr, "Cannot sync event, err %d: %s\n",
		    errno, strerror(errno));
	}

	return err;
}
