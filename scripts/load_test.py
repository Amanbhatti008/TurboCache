import socket
import time
import threading

SERVER = '127.0.0.1'
PORT = 8080
TOTAL_REQUESTS = 100000
CONCURRENT_THREADS = 10  # 10 threads, har thread apna persistent socket rakhega

def worker(thread_id):
    try:
        # 1. Ek hi connection banao (Persistent)
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((SERVER, PORT))
        
        start = time.perf_counter()
        # Increased workload to test high RPS properly
        for i in range(TOTAL_REQUESTS // CONCURRENT_THREADS):
            # SET request
            cmd = f"SET key_{thread_id}_{i} value_{i}\r\n"
            sock.sendall(cmd.encode())
            # GET request
            cmd_get = f"GET key_{thread_id}_{i}\r\n"
            sock.sendall(cmd_get.encode())
            
            resp = sock.recv(1024)
            if b"+OK" not in resp and b"$" not in resp:
                print(f"Error in thread {thread_id}")
        end = time.perf_counter()
        
        sock.close()
        print(f"Thread {thread_id} finished in {end - start:.2f} sec")
    except Exception as e:
        print(f"Thread {thread_id} error: {e}")

if __name__ == "__main__":
    threads = []
    start_time = time.perf_counter()
    for i in range(CONCURRENT_THREADS):
        t = threading.Thread(target=worker, args=(i,))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()
    
    total_time = time.perf_counter() - start_time
    total_ops = TOTAL_REQUESTS * 2 # Since we do SET and GET
    print(f"\nTotal Requests: {total_ops}")
    print(f"Total Time: {total_time:.2f} sec")
    print(f"RPS: {total_ops / total_time:.2f}")
