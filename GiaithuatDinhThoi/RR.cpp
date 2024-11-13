#include <stdio.h>
#include <stdlib.h>
#define SORT_BY_ARRIVAL 0   // Hằng số sắp xếp theo thời gian đến
#define SORT_BY_PID 1       // Hằng số sắp xếp theo PID
#define SORT_BY_BURST 2     // Hằng số sắp xếp theo thời gian burst
#define SORT_BY_START 3     // Hằng số sắp xếp theo thời gian bắt đầu
#define MIN(X, Y)  // Macro lấy giá trị nhỏ nhất

// Cấu trúc PCB (Process Control Block) lưu thông tin tiến trình
typedef struct {
    int iPID;               // PID của tiến trình
    int iArrival, iBurst;   // Thời gian đến và thời gian burst
    int iStart, iFinish;    // Thời gian bắt đầu và kết thúc
    int iWaiting;           // Thời gian chờ
    int iResponse;          // Thời gian đáp ứng
    int iTaT;               // Thời gian hoàn tất (Turnaround Time)
} PCB;

// Hàm nhập thông tin các tiến trình
void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time for process %d: ", i + 1);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = -1; // Khởi tạo iStart là -1 để đánh dấu tiến trình chưa bắt đầu
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

// Hàm in thông tin các tiến trình
void printProcess(int n, PCB P[], int burstmap[]) {
    printf("PID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               P[i].iPID,
               P[i].iArrival,
               burstmap[P[i].iPID],
               P[i].iStart,
               P[i].iFinish,
               P[i].iWaiting,
               P[i].iResponse,
               P[i].iTaT);
    }
}

// Hàm xuất biểu đồ Gantt
void exportGanttChart(int ganttCount, PCB Gantt[]) {
    int i;
    printf("\nBieu do Gantt:\n");
    // In các tiến trình
    for (i = 0; i < ganttCount; i++) {
        printf("|  P%d  ", Gantt[i].iPID);
    }
    printf("|\n");
    // In các thời điểm
    printf("%d", Gantt[0].iStart);
    for (i = 0; i < ganttCount; i++) {
        printf("     %d", Gantt[i].iFinish);
    }
    printf("\n");
}

// Hàm thêm tiến trình vào danh sách
void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;  // Thêm tiến trình Q vào vị trí cuối cùng của mảng P
    (*n)++;     // Tăng số lượng tiến trình trong mảng
}

// Hàm xóa tiến trình khỏi danh sách
void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];  // Dời các phần tử sau index lên trước
    }
    (*n)--;  // Giảm số lượng tiến trình trong mảng
}

// Hàm hoán đổi hai tiến trình
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

// Hàm phân đoạn cho quicksort
int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];  // Chọn phần tử chốt
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        int compare = 0;
        // So sánh theo tiêu chí sắp xếp
        if (iCriteria == SORT_BY_ARRIVAL)
            compare = P[j].iArrival < pivot.iArrival;
        else if (iCriteria == SORT_BY_PID)
            compare = P[j].iPID < pivot.iPID;
        else if (iCriteria == SORT_BY_BURST) {
            if (P[j].iBurst == pivot.iBurst)
                compare = P[j].iArrival < pivot.iArrival;
            else
                compare = P[j].iBurst < pivot.iBurst;
        } else if (iCriteria == SORT_BY_START)
            compare = P[j].iStart < pivot.iStart;
        if (compare) {
            i++;
            swapProcess(&P[i], &P[j]);  // Hoán đổi nếu thỏa mãn điều kiện
        }
    }
    swapProcess(&P[i + 1], &P[high]);  // Đưa phần tử chốt về vị trí đúng
    return i + 1;  // Trả về vị trí của phần tử chốt
}

// Hàm sắp xếp nhanh (quicksort)
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);  // Phân đoạn mảng
        quickSort(P, low, pi - 1, iCriteria);  // Sắp xếp phần bên trái
        quickSort(P, pi + 1, high, iCriteria); // Sắp xếp phần bên phải
    }
}

// Hàm tính thời gian chờ trung bình
void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += P[i].iWaiting;  // Cộng dồn thời gian chờ của từng tiến trình
    }
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
}

// Hàm tính thời gian hoàn thành trung bình
void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++) {
        totalTaT += P[i].iTaT;  // Cộng dồn thời gian hoàn tất của từng tiến trình
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[100];            // Mảng lưu các tiến trình đầu vào
    PCB ReadyQueue[100];       // Hàng đợi sẵn sàng
    PCB TerminatedArray[100];  // Mảng lưu các tiến trình đã hoàn thành
    PCB GanttChart[100];       // Mảng lưu biểu đồ Gantt
    int iNumberOfProcess;     // Số lượng tiến trình
    int burstmap[100];         // Mảng lưu thời gian burst ban đầu của từng tiến trình
    int QTime;                // Thời gian quantum

     // Nhập số lượng tiến trình
    printf("Please input number of processes: ");
    scanf("%d", &iNumberOfProcess);

    // Nhập thời gian quantum
    printf("Please input quantum time: ");
    scanf("%d", &QTime);
   

    int iRemain = iNumberOfProcess; // Số tiến trình còn lại chưa xử lý
    int iReady = 0;                 // Số tiến trình trong hàng đợi sẵn sàng
    int iTerminated = 0;            // Số tiến trình đã hoàn thành
    int iGantt = 0;                 // Số phần tử trong biểu đồ Gantt
    inputProcess(iNumberOfProcess, Input); // Nhập thông tin các tiến trình

    // Lưu thời gian burst ban đầu vào burstmap
    for (int i = 0; i < iNumberOfProcess; i++) {
        burstmap[Input[i].iPID] = Input[i].iBurst;
    }

    // Sắp xếp các tiến trình theo thời gian đến
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    int time = 0; // Biến thời gian hiện tại

    // Nếu thời gian đến của tiến trình đầu tiên lớn hơn thời gian hiện tại
    if (Input[0].iArrival > time) {
        time = Input[0].iArrival; // Cập nhật thời gian hiện tại
    }
    // Thêm tiến trình đầu tiên vào hàng đợi sẵn sàng
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input); // Loại bỏ tiến trình khỏi mảng Input

    // printf("\nReady Queue: ");
    // printProcess(iReady, ReadyQueue, burstmap);

    // Vòng lặp chính xử lý các tiến trình cho đến khi tất cả hoàn thành
    while (iTerminated < iNumberOfProcess) {
        // Kiểm tra các tiến trình mới đến và thêm vào hàng đợi sẵn sàng
        while (iRemain > 0) {
            if (Input[0].iArrival <= time) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            } else
                break;
        }

        if (iReady > 0) {
            PCB currentProcess = ReadyQueue[0]; // Lấy tiến trình đầu tiên trong hàng đợi
            removeProcess(&iReady, 0, ReadyQueue); // Loại bỏ tiến trình khỏi hàng đợi

            // Nếu tiến trình chưa bắt đầu, thiết lập thời gian bắt đầu và thời gian đáp ứng
            if (currentProcess.iStart == -1) {
                currentProcess.iStart = time;
                currentProcess.iResponse = currentProcess.iStart - currentProcess.iArrival;
            }

            // Thực thi tiến trình trong thời gian quantum hoặc thời gian burst còn lại
            int execTime = (((QTime) < (currentProcess.iBurst)) ? (QTime) : (currentProcess.iBurst));
            time += execTime;
            currentProcess.iBurst -= execTime;

            // Lưu tiến trình vào biểu đồ Gantt
            PCB ganttProcess = currentProcess;
            ganttProcess.iBurst = execTime;
            ganttProcess.iStart = time - execTime;
            ganttProcess.iFinish = time;
            pushProcess(&iGantt, GanttChart, ganttProcess);

            // Kiểm tra các tiến trình mới đến trong thời gian thực thi
            while (iRemain > 0) {
                if (Input[0].iArrival <= time) {
                    pushProcess(&iReady, ReadyQueue, Input[0]);
                    removeProcess(&iRemain, 0, Input);
                    continue;
                } else
                    break;
            }

            // Nếu tiến trình chưa hoàn thành, thêm lại vào hàng đợi sẵn sàng
            if (currentProcess.iBurst > 0) {
                pushProcess(&iReady, ReadyQueue, currentProcess);
            } else {
                // Nếu tiến trình đã hoàn thành, tính toán các thông số
                currentProcess.iFinish = time;
                currentProcess.iTaT = currentProcess.iFinish - currentProcess.iArrival;
                currentProcess.iWaiting = currentProcess.iTaT - burstmap[currentProcess.iPID];
                pushProcess(&iTerminated, TerminatedArray, currentProcess);
            }
        } else {
            // Nếu không có tiến trình trong hàng đợi sẵn sàng, tiến tới thời gian đến của tiến trình tiếp theo
            if (iRemain > 0) {
                time = Input[0].iArrival;
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
            }
        }
    }

    printf("\n===== RR Scheduling =====\n");
    exportGanttChart(iGantt, GanttChart); // Xuất biểu đồ Gantt
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID); // Sắp xếp tiến trình đã hoàn thành theo PID
    printProcess(iTerminated, TerminatedArray, burstmap); // In thông tin tiến trình đã hoàn thành
    calculateAWT(iTerminated, TerminatedArray); // Tính thời gian chờ trung bình
    calculateATaT(iTerminated, TerminatedArray); // Tính thời gian hoàn tất trung bình
    return 0;
}
