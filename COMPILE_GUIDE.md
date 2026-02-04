# GCC 컴파일 옵션 상세 설명

## 명령어 분석

```bash
gcc your_program.c -L. -lthreadpool -lpthread -o your_program
```

---

## 각 옵션 설명

### 1. `your_program.c`
- **의미**: 컴파일할 소스 파일
- **역할**: 메인 프로그램의 C 소스 코드

### 2. `-L.` (대문자 L + 점)
- **의미**: Library search path (라이브러리 검색 경로)
- **`.`**: 현재 디렉토리를 라이브러리 검색 경로에 추가
- **역할**: `libthreadpool.a` 파일을 찾을 위치를 지정

**예시:**
```bash
-L.          # 현재 디렉토리
-L/usr/lib   # /usr/lib 디렉토리
-L../lib     # 상위 디렉토리의 lib 폴더
```

### 3. `-lthreadpool` (소문자 l)
- **의미**: Link library (라이브러리 링크)
- **규칙**: `-l` 다음에 오는 이름은 `lib` 접두사와 `.a` 확장자를 자동으로 붙임
- **실제 파일**: `libthreadpool.a`를 찾음
- **검색 순서**: 
  1. `-L`로 지정한 경로
  2. 시스템 기본 경로 (`/usr/lib`, `/lib` 등)

**주의사항:**
- `-lthreadpool` → `libthreadpool.a` 또는 `libthreadpool.so`를 찾음
- 파일명에서 `lib` 접두사와 확장자는 생략

### 4. `-lpthread`
- **의미**: POSIX Thread 라이브러리 링크
- **실제 파일**: `libpthread.so` (동적 라이브러리)
- **역할**: pthread 함수들 (`pthread_create`, `pthread_mutex_lock` 등) 사용 가능

**왜 필요한가?**
- `thread_pool.c`에서 `pthread.h`를 사용하므로 필수
- 시스템 라이브러리이므로 `-L` 옵션 없이도 찾을 수 있음

### 5. `-o your_program`
- **의미**: Output file name (출력 파일명)
- **역할**: 컴파일 결과물의 이름 지정
- **생략 시**: 기본값은 `a.out`

---

## 전체 과정 흐름

```
1. your_program.c 컴파일
   ↓
2. 오브젝트 파일 생성 (your_program.o)
   ↓
3. 라이브러리 검색
   - -L. → 현재 디렉토리에서 libthreadpool.a 찾기
   - -lthreadpool → libthreadpool.a 링크
   - -lpthread → libpthread.so 링크
   ↓
4. 모든 오브젝트 파일과 라이브러리 결합 (링킹)
   ↓
5. 실행 파일 생성 (your_program)
```

---

## 실제 사용 예시

### 예시 1: 기본 사용
```bash
gcc my_server.c -L. -lthreadpool -lpthread -o my_server
```

### 예시 2: 여러 라이브러리 링크
```bash
gcc program.c -L. -lthreadpool -lm -lpthread -o program
# -lm: 수학 라이브러리 (libm.so)
```

### 예시 3: 다른 경로의 라이브러리
```bash
gcc program.c -L../lib -L/usr/local/lib -lthreadpool -lpthread -o program
```

### 예시 4: 직접 파일 지정 (권장하지 않음)
```bash
gcc program.c libthreadpool.a -lpthread -o program
# -L, -l 옵션 없이 직접 파일명 지정
```

---

## 옵션 순서의 중요성

### 올바른 순서
```bash
gcc source.c -L. -lthreadpool -lpthread -o output
```

### 잘못된 순서 (에러 발생 가능)
```bash
gcc -L. -lthreadpool source.c -lpthread -o output
# 소스 파일은 보통 앞에 위치
```

**일반적인 순서:**
1. 소스 파일들
2. `-L` 옵션들 (라이브러리 경로)
3. `-l` 옵션들 (라이브러리 이름)
4. `-o` 옵션 (출력 파일명)

---

## 링크 순서의 중요성

### 의존성 순서
```bash
# 올바른 순서: 의존성이 있는 라이브러리를 나중에
gcc program.c -L. -lthreadpool -lpthread -o program
```

**이유:**
- `libthreadpool.a`가 `libpthread.so`에 의존
- 링커는 **오른쪽에서 왼쪽**으로 의존성을 해결
- 따라서 의존성이 있는 라이브러리(`-lpthread`)를 나중에 배치

### 잘못된 순서 (에러 가능)
```bash
gcc program.c -L. -lpthread -lthreadpool -o program
# 일부 경우 undefined reference 에러 발생 가능
```

---

## 자주 발생하는 에러

### 1. 라이브러리를 찾을 수 없음
```
/usr/bin/ld: cannot find -lthreadpool
```

**해결:**
- `-L.` 옵션 확인 (현재 디렉토리에 `libthreadpool.a`가 있는지)
- 파일명 확인 (`libthreadpool.a`가 맞는지)

### 2. 심볼을 찾을 수 없음
```
undefined reference to `pthread_create'
```

**해결:**
- `-lpthread` 옵션 추가

### 3. 링크 순서 문제
```
undefined reference to `thread_pool_init'
```

**해결:**
- `-lthreadpool` 옵션 확인
- 라이브러리 링크 순서 확인

---

## 디버깅 팁

### 라이브러리 내용 확인
```bash
# 라이브러리에 포함된 오브젝트 파일 확인
ar t libthreadpool.a

# 심볼(함수) 목록 확인
nm libthreadpool.a

# 상세 정보
ar tv libthreadpool.a
```

### 링크 과정 상세 출력
```bash
gcc -v your_program.c -L. -lthreadpool -lpthread -o your_program
# -v 옵션으로 상세한 링크 과정 출력
```

---

## 요약

| 옵션 | 의미 | 예시 |
|------|------|------|
| `-L.` | 라이브러리 검색 경로 추가 | 현재 디렉토리 |
| `-lthreadpool` | `libthreadpool.a` 링크 | `-l` + 이름 |
| `-lpthread` | POSIX Thread 라이브러리 링크 | 시스템 라이브러리 |
| `-o` | 출력 파일명 지정 | 실행 파일 이름 |

**핵심 포인트:**
- `-L`: 라이브러리를 **어디서** 찾을지
- `-l`: **어떤** 라이브러리를 링크할지
- 순서: 소스 → 경로(`-L`) → 라이브러리(`-l`) → 출력(`-o`)

