#ifndef  __NONCOPYABLE_H__
#define  __NONCOPYABLE_H__

#define NONCOPYABLE(Type) 				\
  Type(const Type&);                    \
  Type& operator=(const Type&)


#endif  /*__NONCOPYABLE_H__*/
