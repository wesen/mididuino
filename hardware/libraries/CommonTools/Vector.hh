#ifndef VECTOR_H__
#define VECTOR_H__

template <class T, int N>
class Vector {
protected:
public:
  int size;
  T arr[N];


  Vector() {
    size = N;
    for (int i = 0; i < N; i++) {
      arr[i] = (T)0;
    }
  }

  bool add(T t) {
    for (int i = 0; i < N; i++) {
      if (arr[i] == (T)0 || arr[i] == t) {
	arr[i] = t;
	return true;
      }
    }
    return false;
  }

  bool remove(T t) {
    for (int i = 0; i < N; i++) {
      if (arr[i] == t) {
	arr[i] = (T)0;
	return true;
      }
    }
    return false;
  }
};

#endif /* VECTOR_H__ */
