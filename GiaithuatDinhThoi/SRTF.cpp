#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0  // Sắp xếp theo thời gian đến
#define SORT_BY_PID 1      // Sắp xếp theo PID
#define SORT_BY_BURST 2    // Sắp xếp theo thời gian thực thi
#define SORT_BY_REMAINING_BURST 3 // Sắp xếp theo thời gian thực thi còn lại

typedef struct {
    int iPID;              // ID của tiến trình
    int iArrival;          // Thời gian đến
    int iBurst;            // Thời gian thực thi
    int iStart;            // Thời gian bắt đầu
    int iFinish;           // Thời gian kết thúc
    int iWaiting;          // Thời gian chờ
    int iResponse;         // Thời gian phản hồi
    int iTaT;              // Thời gian quay vòng
    int iRemainingBurst;   // Thời gian thực thi còn lại
    int isCompleted;       // Trạng thái hoàn thành
} PCB;

void inputProcess(int n, PCB P[]);
void printProcess(int n, PCB P[]);
void exportGanttChart(PCB Gantt[], int ganttCount);
void pushProcess(int *n, PCB P[], PCB Q);
void removeProcess(int *n, int index, PCB P[]);
int swapProcess(PCB *P, PCB *Q);
int partition(PCB P[], int low, int high, int iCriteria);
void quickSort(PCB P[], int low, int high, int iCriteria);
void calculateAWT(int n, PCB P[]);
void calculateATaT(int n, PCB P[]);

int main() {
    PCB Input[100];          // Mảng lưu trữ các tiến trình đầu vào
    PCB ReadyQueue[100];     // Hàng đợi sẵn sàng
    PCB TerminatedArray[100]; // Mảng lưu trữ các tiến trình đã hoàn thành
    PCB GanttChart[100];    // Mảng lưu trữ biểu đồ Gantt
    int ganttCount = 0;     // Số lượng mục trong biểu đồ Gantt
    int iNumberOfProcess;   // Số lượng tiến trình

    printf("Please input number of processes: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess; // Số tiến trình còn lại trong Input
    int iReady = 0;                 // Số tiến trình trong ReadyQueue
    int iTerminated = 0;            // Số tiến trình đã hoàn thành
    int iCurrentTime = 0;           // Thời gian hiện tại
    int i, shortestIndex;

    inputProcess(iNumberOfProcess, Input); // Nhập thông tin các tiến trình
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL); // Sắp xếp theo thời gian đến

    // Khởi tạo các tiến trình
    for (i = 0; i < iNumberOfProcess; i++) {
        Input[i].iRemainingBurst = Input[i].iBurst; // Thời gian thực thi còn lại ban đầu bằng thời gian thực thi
        Input[i].isCompleted = 0;                   // Chưa hoàn thành
    }

    // Vòng lặp chính
    while (iTerminated < iNumberOfProcess) {
        // Di chuyển các tiến trình đã đến vào ReadyQueue
        while (iRemain > 0 && Input[0].iArrival <= iCurrentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
            if (iRemain > 0) {
                quickSort(Input, 0, iRemain - 1, SORT_BY_ARRIVAL);
            }
        }

        // Tìm tiến trình có thời gian thực thi còn lại ngắn nhất
        shortestIndex = -1;
        int shortestRemainingTime = 99999;
        for (i = 0; i < iReady; i++) {
            if (ReadyQueue[i].iRemainingBurst < shortestRemainingTime &&
                ReadyQueue[i].iArrival <= iCurrentTime) {
                shortestRemainingTime = ReadyQueue[i].iRemainingBurst;
                shortestIndex = i;
            }
        }

        if (shortestIndex != -1) {
            // Nếu tiến trình chưa bắt đầu, thiết lập thời gian bắt đầu và thời gian phản hồi
            if (ReadyQueue[shortestIndex].iRemainingBurst == ReadyQueue[shortestIndex].iBurst) {
                ReadyQueue[shortestIndex].iStart = iCurrentTime;
                ReadyQueue[shortestIndex].iResponse = ReadyQueue[shortestIndex].iStart - ReadyQueue[shortestIndex].iArrival;
            }

            // Ghi lại thông tin cho biểu đồ Gantt
            if (ganttCount > 0 && GanttChart[ganttCount - 1].iPID == ReadyQueue[shortestIndex].iPID) {
                GanttChart[ganttCount - 1].iFinish = iCurrentTime + 1;
            } else {
                PCB ganttEntry = ReadyQueue[shortestIndex];
                ganttEntry.iStart = iCurrentTime;
                ganttEntry.iFinish = iCurrentTime + 1;
                pushProcess(&ganttCount, GanttChart, ganttEntry);
            }

            // Thực thi tiến trình
            ReadyQueue[shortestIndex].iRemainingBurst--;
            iCurrentTime++;

            // Kiểm tra nếu tiến trình đã hoàn thành
            if (ReadyQueue[shortestIndex].iRemainingBurst == 0) {
                ReadyQueue[shortestIndex].iFinish = iCurrentTime;
                ReadyQueue[shortestIndex].iTaT = ReadyQueue[shortestIndex].iFinish - ReadyQueue[shortestIndex].iArrival;
                ReadyQueue[shortestIndex].iWaiting = ReadyQueue[shortestIndex].iTaT - ReadyQueue[shortestIndex].iBurst;
                ReadyQueue[shortestIndex].isCompleted = 1;

                // Đưa tiến trình vào mảng hoàn thành
                pushProcess(&iTerminated, TerminatedArray, ReadyQueue[shortestIndex]);
                removeProcess(&iReady, shortestIndex, ReadyQueue);
            }
        } else {
            // Nếu không có tiến trình nào trong ReadyQueue, tăng thời gian hiện tại
            iCurrentTime++;
        }
    }

    // Xuất kết quả
    printf("\n===== SRTF Scheduling =====\n");
    exportGanttChart(GanttChart, ganttCount); // Xuất biểu đồ Gantt
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID); // Sắp xếp theo PID
    printProcess(iTerminated, TerminatedArray); // In thông tin tiến trình
    calculateAWT(iTerminated, TerminatedArray); // Tính thời gian chờ trung bình
    calculateATaT(iTerminated, TerminatedArray); // Tính thời gian quay vòng trung bình

    return 0;
}

// Hàm nhập thông tin các tiến trình
void inputProcess(int n, PCB P[]) {
    int i;
    for (i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time for process %d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = -1;
        P[i].iFinish = -1;
        P[i].iWaiting = -1;
        P[i].iResponse = -1;
        P[i].iTaT = -1;
        P[i].iRemainingBurst = P[i].iBurst;
        P[i].isCompleted = 0;
    }
}

// Hàm in thông tin các tiến trình
void printProcess(int n, PCB P[]) {
    int i;
    printf("\nPID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTurnaround\n");
    for (i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart,
               P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}

// Hàm xuất biểu đồ Gantt với thời gian bên dưới
void exportGanttChart(PCB Gantt[], int ganttCount) {
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
        printf("      %d", Gantt[i].iFinish);
    }
    printf("\n");
}

// Hàm thêm tiến trình vào mảng
void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

// Hàm xóa tiến trình khỏi mảng
void removeProcess(int *n, int index, PCB P[]) {
    int i;
    for (i = index; i < *n - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

// Hàm hoán đổi hai tiến trình
int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 0;
}

// Hàm phân hoạch cho thuật toán QuickSort
int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    int j;
    for (j = low; j < high; j++) {
        int condition = 0;
        switch (iCriteria) {
            case SORT_BY_ARRIVAL:
                condition = (P[j].iArrival < pivot.iArrival);
                break;
            case SORT_BY_PID:
                condition = (P[j].iPID < pivot.iPID);
                break;
            case SORT_BY_BURST:
                condition = (P[j].iBurst < pivot.iBurst);
                break;
            case SORT_BY_REMAINING_BURST:
                condition = (P[j].iRemainingBurst < pivot.iRemainingBurst);
                break;
            default:
                break;
        }
        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

// Hàm QuickSort để sắp xếp mảng tiến trình
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

// Hàm tính thời gian chờ trung bình
void calculateAWT(int n, PCB P[]) {
    int i;
    float totalWaitingTime = 0;
    for (i = 0; i < n; i++) {
        totalWaitingTime += P[i].iWaiting;
    }
    printf("\nAverage Waiting Time: %.2f\n", totalWaitingTime / n);
}

// Hàm tính thời gian quay vòng trung bình
void calculateATaT(int n, PCB P[]) {
    int i;
    float totalTurnaroundTime = 0;
    for (i = 0; i < n; i++) {
        totalTurnaroundTime += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", totalTurnaroundTime / n);
}
