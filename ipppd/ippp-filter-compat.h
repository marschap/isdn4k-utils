#ifndef _IPPP_FILTER_COMPAT_H
#define _IPPP_FILTER_COMPAT_H

#ifndef PPPIOCSPASS
#define PPPIOCSPASS	_IOW('t', 71, struct sock_fprog) /* set pass filter */
#endif
#ifndef PPPIOCSACTIVE
#define PPPIOCSACTIVE	_IOW('t', 70, struct sock_fprog) /* set active filt */
#endif
                                                                                
#endif /* _IPPP_FILTER_COMPAT_H */
