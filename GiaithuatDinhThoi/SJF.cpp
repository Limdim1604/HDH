#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0  // Sắp xếp theo thời gian đến
#define SORT_BY_PID 1      // Sắp xếp theo PID
#define SORT_BY_BURST 2    // Sắp xếp theo thời gian thực thi
#define SORT_BY_START 3    // Sắp xếp theo thời gian bắt đầu

// Định nghĩa cấu trúc PCB (Process Control Block)
typedef struct {
    int iPID;              // ID của tiến trình
    int iArrival;          // Thời gian đến
    int iBurst;            // Thời gian thực thi
    int iStart;            // Thời gian bắt đầu
    int iFinish;           // Thời gian kết thúc
    int iWaiting;          // Thời gian chờ
    int iResponse;         // Thời gian phản hồi
    int iTaT;              // Thời gian quay vòng (Turnaround Time)
} PCB;

// Khai báo các hàm sử dụng trong chương trình
void inputProcess(int n, PCB P[]);
void printProcess(int n, PCB P[]);
void exportGanttChart(int n, PCB P[]);
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
    int iNumberOfProcess;   // Số lượng tiến trình

    printf("Please input number of processes: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess; // Số tiến trình còn lại chưa xử lý
    int iReady = 0;                 // Số tiến trình trong hàng đợi sẵn sàng
    int iTerminated = 0;            // Số tiến trình đã hoàn thành
    int iCurrentTime = 0;           // Thời gian hiện tại

    inputProcess(iNumberOfProcess, Input); // Nhập thông tin các tiến trình
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL); // Sắp xếp tiến trình theo thời gian đến

    // Vòng lặp chính để xử lý các tiến trình
    while (iTerminated < iNumberOfProcess) {
        // Di chuyển các tiến trình đã đến vào hàng đợi sẵn sàng
        while (iRemain > 0 && Input[0].iArrival <= iCurrentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);     // Đưa tiến trình vào hàng đợi sẵn sàng
            removeProcess(&iRemain, 0, Input);              // Xóa tiến trình khỏi mảng Input
            if (iRemain > 0) {
                quickSort(Input, 0, iRemain - 1, SORT_BY_ARRIVAL); // Sắp xếp lại mảng Input
            }
            quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);   // Sắp xếp hàng đợi sẵn sàng theo thời gian thực thi
        }

        if (iReady > 0) {
            // Lấy tiến trình có thời gian thực thi ngắn nhất
            PCB currentProcess = ReadyQueue[0];

            // Lên lịch cho tiến trình hiện tại
            currentProcess.iStart = iCurrentTime;                     // Thời gian bắt đầu
            currentProcess.iFinish = currentProcess.iStart + currentProcess.iBurst; // Thời gian kết thúc
            currentProcess.iResponse = currentProcess.iStart - currentProcess.iArrival; // Thời gian phản hồi
            currentProcess.iWaiting = currentProcess.iResponse;       // Thời gian chờ
            currentProcess.iTaT = currentProcess.iFinish - currentProcess.iArrival;     // Thời gian quay vòng

            iCurrentTime = currentProcess.iFinish; // Cập nhật thời gian hiện tại

            pushProcess(&iTerminated, TerminatedArray, currentProcess); // Đưa tiến trình vào mảng đã hoàn thành
            removeProcess(&iReady, 0, ReadyQueue); // Xóa tiến trình khỏi hàng đợi sẵn sàng
        } else {
            // Nếu không có tiến trình nào sẵn sàng, tăng thời gian hiện tại
            iCurrentTime++;
        }
    }

    // Xuất kết quả
    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray); // Xuất biểu đồ Gantt
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID); // Sắp xếp tiến trình theo PID
    printProcess(iTerminated, TerminatedArray); // In thông tin các tiến trình
    calculateAWT(iTerminated, TerminatedArray); // Tính thời gian chờ trung bình
    calculateATaT(iTerminated, TerminatedArray); // Tính thời gian quay vòng trung bình

    return 0;
}

// Hàm nhập thông tin các tiến trình
void inputProcess(int n, PCB P[]) {
    int i;
    for (i = 0; i < n; i++) {
        P[i].iPID = i + 1; // Gán PID cho tiến trình
        printf("Enter arrival time and burst time for process %d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        // Khởi tạo các giá trị ban đầu
        P[i].iStart = -1;
        P[i].iFinish = -1;
        P[i].iWaiting = -1;
        P[i].iResponse = -1;
        P[i].iTaT = -1;
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

// Hàm xuất biểu đồ Gantt
void exportGanttChart(int n, PCB P[]) {
    int i;
    printf("\nBieu do Gantt:\n");
    for (i = 0; i < n; i++) {
        printf("| P%d ", P[i].iPID);
    }
    printf("|\n");

    // In thời gian bên dưới biểu đồ
    printf("0");
    for (i = 0; i < n; i++) {
        printf("   %d", P[i].iFinish);
    }
    printf("\n");
}

// Hàm thêm tiến trình vào mảng
void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q; // Thêm tiến trình vào cuối mảng
    (*n)++;    // Tăng số lượng tiến trình
}

// Hàm xóa tiến trình khỏi mảng
void removeProcess(int *n, int index, PCB P[]) {
    int i;
    for (i = index; i < *n - 1; i++) {
        P[i] = P[i + 1]; // Dịch chuyển các phần tử lên trước
    }
    (*n)--; // Giảm số lượng tiến trình
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
    PCB pivot = P[high]; // Chọn phần tử chốt
    int i = low - 1;
    int j;
    for (j = low; j < high; j++) {
        int condition = 0;
        // Xác định điều kiện sắp xếp dựa trên tiêu chí
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
            case SORT_BY_START:
                condition = (P[j].iStart < pivot.iStart);
                break;
            default:
                break;
        }
        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]); // Hoán đổi các phần tử
        }
    }
    swapProcess(&P[i + 1], &P[high]); // Đưa phần tử chốt về vị trí đúng
    return i + 1; // Trả về chỉ số của phần tử chốt
}

// Hàm QuickSort để sắp xếp mảng tiến trình
void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria); // Phân hoạch mảng
        quickSort(P, low, pi - 1, iCriteria);        // Sắp xếp phần bên trái
        quickSort(P, pi + 1, high, iCriteria);       // Sắp xếp phần bên phải
    }
}

// Hàm tính thời gian chờ trung bình
void calculateAWT(int n, PCB P[]) {
    int i;
    float totalWaitingTime = 0;
    for (i = 0; i < n; i++) {
        totalWaitingTime += P[i].iWaiting; // Cộng dồn thời gian chờ
    }
    printf("\nAverage Waiting Time: %.2f\n", totalWaitingTime / n);
}

// Hàm tính thời gian quay vòng trung bình
void calculateATaT(int n, PCB P[]) {
    int i;
    float totalTurnaroundTime = 0;
    for (i = 0; i < n; i++) {
        totalTurnaroundTime += P[i].iTaT; // Cộng dồn thời gian quay vòng
    }
    printf("\nAverage Turnaround Time: %.2f\n", totalTurnaroundTime / n);
}
