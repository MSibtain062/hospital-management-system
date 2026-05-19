#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <limits>
#include <stdexcept>
using namespace std;

struct InputState {
    HWND hwnd;
    HWND hEdit;
    bool ok;
    string prompt;
    string result;
};

struct LoginState {
    HWND hwnd;
    HWND hUsernameEdit;
    HWND hPasswordEdit;
    bool ok;
    string username;
    string password;
};

const char G_INPUT_CLASS[] = "HMS_InputBox_Class";
const char G_LOGIN_CLASS[] = "HMS_Login_Class";

LRESULT CALLBACK InputWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    InputState* state = reinterpret_cast<InputState*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_CREATE: {
            CREATESTRUCTA* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
            state = reinterpret_cast<InputState*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));

            CreateWindowExA(0, "STATIC", state->prompt.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
                            10, 10, 360, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

            state->hEdit = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                           10, 35, 360, 24, hwnd, reinterpret_cast<HMENU>(1001), GetModuleHandle(NULL), NULL);

            CreateWindowExA(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            100, 70, 80, 26, hwnd, reinterpret_cast<HMENU>(1002), GetModuleHandle(NULL), NULL);

            CreateWindowExA(0, "BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            200, 70, 80, 26, hwnd, reinterpret_cast<HMENU>(1003), GetModuleHandle(NULL), NULL);
            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == 1002) {
                char buffer[512] = {};
                GetWindowTextA(state->hEdit, buffer, sizeof(buffer));
                state->result = buffer;
                state->ok = true;
                DestroyWindow(hwnd);
                return 0;
            } else if (LOWORD(wParam) == 1003) {
                state->ok = false;
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_CLOSE:
            state->ok = false;
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterInputClass(HINSTANCE hInstance) {
    WNDCLASSA wc{};
    wc.lpfnWndProc = InputWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = G_INPUT_CLASS;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    return RegisterClassA(&wc) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK LoginWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LoginState* state = reinterpret_cast<LoginState*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_CREATE: {
            CREATESTRUCTA* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
            state = reinterpret_cast<LoginState*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));

            CreateWindowExA(0, "STATIC", "Username:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                            20, 20, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

            state->hUsernameEdit = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                                    20, 45, 260, 24, hwnd, reinterpret_cast<HMENU>(3001), GetModuleHandle(NULL), NULL);

            CreateWindowExA(0, "STATIC", "Password:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                            20, 80, 100, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

            state->hPasswordEdit = CreateWindowExA(0, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD,
                                                    20, 105, 260, 24, hwnd, reinterpret_cast<HMENU>(3002), GetModuleHandle(NULL), NULL);

            CreateWindowExA(0, "BUTTON", "Login", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            80, 150, 80, 26, hwnd, reinterpret_cast<HMENU>(3003), GetModuleHandle(NULL), NULL);

            CreateWindowExA(0, "BUTTON", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            180, 150, 80, 26, hwnd, reinterpret_cast<HMENU>(3004), GetModuleHandle(NULL), NULL);
            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == 3003) {
                char usernameBuffer[256] = {};
                char passwordBuffer[256] = {};
                GetWindowTextA(state->hUsernameEdit, usernameBuffer, sizeof(usernameBuffer));
                GetWindowTextA(state->hPasswordEdit, passwordBuffer, sizeof(passwordBuffer));
                
                state->username = usernameBuffer;
                state->password = passwordBuffer;
                
                if (state->username == "admin" && state->password == "admin123123") {
                    state->ok = true;
                    DestroyWindow(hwnd);
                } else {
                    MessageBoxA(hwnd, "Invalid username or password!", "Login Failed", MB_OK | MB_ICONERROR);
                    SetWindowTextA(state->hPasswordEdit, "");
                    SetFocus(state->hUsernameEdit);
                }
                return 0;
            } else if (LOWORD(wParam) == 3004) {
                state->ok = false;
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_CLOSE:
            state->ok = false;
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool RegisterLoginClass(HINSTANCE hInstance) {
    WNDCLASSA wc{};
    wc.lpfnWndProc = LoginWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = G_LOGIN_CLASS;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    return RegisterClassA(&wc) != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool GuiShowLoginDialog(const char* title) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!RegisterLoginClass(hInstance)) {
        return false;
    }

    LoginState state{};
    state.ok = false;

    HWND dlg = CreateWindowExA(0, G_LOGIN_CLASS, title,
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                               CW_USEDEFAULT, CW_USEDEFAULT, 320, 220,
                               NULL, NULL, hInstance, &state);
    if (!dlg) {
        return false;
    }

    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);
    SetFocus(state.hUsernameEdit);

    MSG msg = {};
    while (IsWindow(dlg) && GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == dlg || IsChild(dlg, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return state.ok;
}

bool GuiInputText(HWND owner, const char* title, const char* prompt, string& out) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!RegisterInputClass(hInstance)) {
        return false;
    }

    InputState state{};
    state.ok = false;
    state.prompt = prompt;

    HWND dlg = CreateWindowExA(0, G_INPUT_CLASS, title,
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                               CW_USEDEFAULT, CW_USEDEFAULT, 400, 140,
                               owner, NULL, hInstance, &state);
    if (!dlg) {
        return false;
    }

    ShowWindow(dlg, SW_SHOW);
    UpdateWindow(dlg);
    SetFocus(state.hEdit);

    MSG msg = {};
    while (IsWindow(dlg) && GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.hwnd == dlg || IsChild(dlg, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    out = state.result;
    return state.ok;
}

bool GuiInputInt(HWND owner, const char* title, const char* prompt, int& value) {
    string text;
    if (!GuiInputText(owner, title, prompt, text)) {
        return false;
    }

    try {
        size_t pos;
        int temp = stoi(text, &pos);
        if (pos != text.size()) {
            throw invalid_argument("not numeric");
        }
        value = temp;
        return true;
    } catch (...) {
        MessageBoxA(owner, "Please enter a valid whole number.", "Invalid Input", MB_OK | MB_ICONERROR);
        return false;
    }
}

void GuiShowMessage(HWND owner, const string& text, const char* title = "Info") {
    MessageBoxA(owner, text.c_str(), title, MB_OK | MB_ICONINFORMATION);
}

bool isAlphaSpace(const string& text) {
    for (char ch : text) {
        if (!isalpha(static_cast<unsigned char>(ch)) && ch != ' ') {
            return false;
        }
    }
    return !text.empty();
}

bool isDigits(const string& text) {
    for (char ch : text) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return !text.empty();
}

bool isBloodGroup(const string& text) {
    if (text.empty()) return false;
    for (char ch : text) {
        if (!isalnum(static_cast<unsigned char>(ch)) && ch != '+' && ch != '-') {
            return false;
        }
    }
    return true;
}

class Person {
protected:
    int id;
    string name;
    int age;
    string gender;

public:
    Person() : id(0), age(0) {}
    Person(int i, string n, int a, string g)
        : id(i), name(std::move(n)), age(a), gender(std::move(g)) {}
    virtual ~Person() {}
    virtual void displayInfo() = 0;
    int getID() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getGender() const { return gender; }
};

class Patient : public Person {
private:
    string disease;
    string bloodGroup;

public:
    Patient() {}
    Patient(int i, string n, int a, string g, string d, string bg)
        : Person(i, std::move(n), a, std::move(g)), disease(std::move(d)), bloodGroup(std::move(bg)) {}

    void displayInfo() override {
        cout << "\n===================================\n";
        cout << "PATIENT INFORMATION\n";
        cout << "===================================\n";
        cout << "ID          : " << id << "\n";
        cout << "Name        : " << name << "\n";
        cout << "Age         : " << age << "\n";
        cout << "Gender      : " << gender << "\n";
        cout << "Disease     : " << disease << "\n";
        cout << "Blood Group : " << bloodGroup << "\n";
        cout << "===================================\n";
    }
    string getDisease() const { return disease; }
    string getBloodGroup() const { return bloodGroup; }
};

class Doctor : public Person {
private:
    string specialization;
    int experience;

public:
    Doctor() {}
    Doctor(int i, string n, int a, string g, string s, int e)
        : Person(i, std::move(n), a, std::move(g)), specialization(std::move(s)), experience(e) {}

    void displayInfo() override {
        cout << "\n===================================\n";
        cout << "DOCTOR INFORMATION\n";
        cout << "===================================\n";
        cout << "ID             : " << id << "\n";
        cout << "Name           : " << name << "\n";
        cout << "Age            : " << age << "\n";
        cout << "Gender         : " << gender << "\n";
        cout << "Specialization : " << specialization << "\n";
        cout << "Experience     : " << experience << " Years\n";
        cout << "===================================\n";
    }
    string getSpecialization() const { return specialization; }
    int getExperience() const { return experience; }
};

class Appointment {
private:
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string time;

public:
    Appointment() : appointmentID(0), patientID(0), doctorID(0) {}
    Appointment(int aid, int pid, int did, string d, string t)
        : appointmentID(aid), patientID(pid), doctorID(did), date(std::move(d)), time(std::move(t)) {}
    int getAppointmentID() const { return appointmentID; }
    int getPatientID() const { return patientID; }
    int getDoctorID() const { return doctorID; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    void displayAppointment() {
        cout << "\n===================================\n";
        cout << "APPOINTMENT DETAILS\n";
        cout << "===================================\n";
        cout << "Appointment ID : " << appointmentID << "\n";
        cout << "Patient ID     : " << patientID << "\n";
        cout << "Doctor ID      : " << doctorID << "\n";
        cout << "Date           : " << date << "\n";
        cout << "Time           : " << time << "\n";
        cout << "===================================\n";
    }
};

class Hospital {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    vector<Appointment> appointments;

    bool promptInt(HWND owner, const char* prompt, int& value, const char* title = "Input") {
        while (true) {
            if (!GuiInputInt(owner, title, prompt, value)) {
                return false;
            }
            if (value <= 0) {
                MessageBoxA(owner, "Value must be greater than zero.", "Invalid Input", MB_OK | MB_ICONERROR);
                continue;
            }
            return true;
        }
    }

    bool promptString(HWND owner, const char* prompt, string& value, bool allowSpaces, const char* title = "Input") {
        while (true) {
            if (!GuiInputText(owner, title, prompt, value)) {
                return false;
            }
            if (value.empty()) {
                MessageBoxA(owner, "This field cannot be empty.", "Invalid Input", MB_OK | MB_ICONERROR);
                continue;
            }
            bool valid = allowSpaces ? isAlphaSpace(value) : isDigits(value);
            if (!valid) {
                MessageBoxA(owner, "Please enter a valid value for this field.", "Invalid Input", MB_OK | MB_ICONERROR);
                continue;
            }
            return true;
        }
    }

    bool promptPatientName(HWND owner, string& value) {
        while (true) {
            if (!GuiInputText(owner, "Input", "Enter Name:", value)) {
                return false;
            }
            if (isAlphaSpace(value)) {
                return true;
            }
            MessageBoxA(owner, "Name must contain only letters and spaces.", "Invalid Input", MB_OK | MB_ICONERROR);
        }
    }

    bool promptGender(HWND owner, string& value) {
        while (true) {
            if (!GuiInputText(owner, "Input", "Enter Gender:", value)) {
                return false;
            }
            if (isAlphaSpace(value)) {
                return true;
            }
            MessageBoxA(owner, "Gender must contain only letters and spaces.", "Invalid Input", MB_OK | MB_ICONERROR);
        }
    }

    bool promptDisease(HWND owner, string& value) {
        while (true) {
            if (!GuiInputText(owner, "Input", "Enter Disease:", value)) {
                return false;
            }
            if (!value.empty()) {
                return true;
            }
            MessageBoxA(owner, "Disease cannot be empty.", "Invalid Input", MB_OK | MB_ICONERROR);
        }
    }

    bool promptBloodGroup(HWND owner, string& value) {
        while (true) {
            if (!GuiInputText(owner, "Input", "Enter Blood Group:", value)) {
                return false;
            }
            if (isBloodGroup(value)) {
                return true;
            }
            MessageBoxA(owner, "Blood group must contain letters, digits, + or -.", "Invalid Input", MB_OK | MB_ICONERROR);
        }
    }

    bool promptSpecialization(HWND owner, string& value) {
        while (true) {
            if (!GuiInputText(owner, "Input", "Enter Specialization:", value)) {
                return false;
            }
            if (isAlphaSpace(value)) {
                return true;
            }
            MessageBoxA(owner, "Specialization must contain only letters and spaces.", "Invalid Input", MB_OK | MB_ICONERROR);
        }
    }

public:
    void addPatient(HWND owner = NULL) {
        int id = 0;
        while (true) {
            if (!promptInt(owner, "Enter Patient ID:", id)) {
                return;
            }
            bool exists = false;
            for (const auto& p : patients) {
                if (p.getID() == id) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                MessageBoxA(owner, "Patient ID already exists. Please choose another.", "Duplicate ID", MB_OK | MB_ICONERROR);
                continue;
            }
            break;
        }

        string name;
        if (!promptPatientName(owner, name)) {
            return;
        }

        int age = 0;
        if (!promptInt(owner, "Enter Age:", age)) {
            return;
        }

        string gender;
        if (!promptGender(owner, gender)) {
            return;
        }

        string disease;
        if (!promptDisease(owner, disease)) {
            return;
        }

        string bloodGroup;
        if (!promptBloodGroup(owner, bloodGroup)) {
            return;
        }

        patients.emplace_back(id, name, age, gender, disease, bloodGroup);
        savePatientsToFile(owner);
        GuiShowMessage(owner, "Patient Added Successfully!", "Success");
    }

    void viewPatients(HWND owner = NULL) {
        if (patients.empty()) {
            GuiShowMessage(owner, "No Patients Found!", "Patients");
            return;
        }

        ostringstream ss;
        for (const auto& p : patients) {
            ss << "===================================\n";
            ss << "PATIENT INFORMATION\n";
            ss << "===================================\n";
            ss << "ID          : " << p.getID() << "\n";
            ss << "Name        : " << p.getName() << "\n";
            ss << "Age         : " << p.getAge() << "\n";
            ss << "Gender      : " << p.getGender() << "\n";
            ss << "Disease     : " << p.getDisease() << "\n";
            ss << "Blood Group : " << p.getBloodGroup() << "\n\n";
        }
        GuiShowMessage(owner, ss.str(), "Patients");
    }

    void searchPatient(HWND owner = NULL) {
        int id = 0;
        if (!promptInt(owner, "Enter Patient ID to Search:", id)) {
            return;
        }

        ostringstream ss;
        bool found = false;
        for (const auto& p : patients) {
            if (p.getID() == id) {
                ss << "===================================\n";
                ss << "PATIENT INFORMATION\n";
                ss << "===================================\n";
                ss << "ID          : " << p.getID() << "\n";
                ss << "Name        : " << p.getName() << "\n";
                ss << "Age         : " << p.getAge() << "\n";
                ss << "Gender      : " << p.getGender() << "\n";
                ss << "Disease     : " << p.getDisease() << "\n";
                ss << "Blood Group : " << p.getBloodGroup() << "\n";
                found = true;
                break;
            }
        }

        if (found) {
            GuiShowMessage(owner, ss.str(), "Patient Found");
        } else {
            GuiShowMessage(owner, "Patient Not Found!", "Search Result");
        }
    }

    void deletePatient(HWND owner = NULL) {
        int id = 0;
        if (!promptInt(owner, "Enter Patient ID to Delete:", id)) {
            return;
        }

        bool found = false;
        for (auto it = patients.begin(); it != patients.end(); ++it) {
            if (it->getID() == id) {
                patients.erase(it);
                found = true;
                break;
            }
        }

        if (found) {
            GuiShowMessage(owner, "Patient Deleted Successfully!", "Success");
        } else {
            GuiShowMessage(owner, "Patient Not Found!", "Delete Result");
        }
    }

    void addDoctor(HWND owner = NULL) {
        int id = 0;
        while (true) {
            if (!promptInt(owner, "Enter Doctor ID:", id)) {
                return;
            }
            bool exists = false;
            for (const auto& d : doctors) {
                if (d.getID() == id) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                MessageBoxA(owner, "Doctor ID already exists. Please choose another.", "Duplicate ID", MB_OK | MB_ICONERROR);
                continue;
            }
            break;
        }

        string name;
        if (!promptPatientName(owner, name)) {
            return;
        }

        int age = 0;
        if (!promptInt(owner, "Enter Age:", age)) {
            return;
        }

        string gender;
        if (!promptGender(owner, gender)) {
            return;
        }

        string specialization;
        if (!promptSpecialization(owner, specialization)) {
            return;
        }

        int experience = 0;
        if (!promptInt(owner, "Enter Experience (Years):", experience)) {
            return;
        }

        doctors.emplace_back(id, name, age, gender, specialization, experience);
        saveDoctorsToFile(owner);
        GuiShowMessage(owner, "Doctor Added Successfully!", "Success");
    }

    void viewDoctors(HWND owner = NULL) {
        if (doctors.empty()) {
            GuiShowMessage(owner, "No Doctors Found!", "Doctors");
            return;
        }

        ostringstream ss;
        for (const auto& d : doctors) {
            ss << "===================================\n";
            ss << "DOCTOR INFORMATION\n";
            ss << "===================================\n";
            ss << "ID             : " << d.getID() << "\n";
            ss << "Name           : " << d.getName() << "\n";
            ss << "Age            : " << d.getAge() << "\n";
            ss << "Gender         : " << d.getGender() << "\n";
            ss << "Specialization : " << d.getSpecialization() << "\n";
            ss << "Experience     : " << d.getExperience() << " Years\n\n";
        }
        GuiShowMessage(owner, ss.str(), "Doctors");
    }

    void searchDoctor(HWND owner = NULL) {
        int id = 0;
        if (!promptInt(owner, "Enter Doctor ID to Search:", id)) {
            return;
        }

        ostringstream ss;
        bool found = false;
        for (const auto& d : doctors) {
            if (d.getID() == id) {
                ss << "===================================\n";
                ss << "DOCTOR INFORMATION\n";
                ss << "===================================\n";
                ss << "ID             : " << d.getID() << "\n";
                ss << "Name           : " << d.getName() << "\n";
                ss << "Age            : " << d.getAge() << "\n";
                ss << "Gender         : " << d.getGender() << "\n";
                ss << "Specialization : " << d.getSpecialization() << "\n";
                ss << "Experience     : " << d.getExperience() << " Years\n";
                found = true;
                break;
            }
        }

        if (found) {
            GuiShowMessage(owner, ss.str(), "Doctor Found");
        } else {
            GuiShowMessage(owner, "Doctor Not Found!", "Search Result");
        }
    }

    void bookAppointment(HWND owner = NULL) {
        int aid = 0;
        if (!promptInt(owner, "Enter Appointment ID:", aid)) {
            return;
        }

        int pid = 0;
        if (!promptInt(owner, "Enter Patient ID:", pid)) {
            return;
        }

        bool patientFound = false;
        for (const auto& p : patients) {
            if (p.getID() == pid) {
                patientFound = true;
                break;
            }
        }
        if (!patientFound) {
            GuiShowMessage(owner, "Patient ID does not exist!", "Error");
            return;
        }

        int did = 0;
        if (!promptInt(owner, "Enter Doctor ID:", did)) {
            return;
        }

        bool doctorFound = false;
        for (const auto& d : doctors) {
            if (d.getID() == did) {
                doctorFound = true;
                break;
            }
        }
        if (!doctorFound) {
            GuiShowMessage(owner, "Doctor ID does not exist!", "Error");
            return;
        }

        string date;
        if (!GuiInputText(owner, "Input", "Enter Appointment Date:", date) || date.empty()) {
            GuiShowMessage(owner, "Appointment date cannot be empty.", "Invalid Input");
            return;
        }

        string time;
        if (!GuiInputText(owner, "Input", "Enter Appointment Time:", time) || time.empty()) {
            GuiShowMessage(owner, "Appointment time cannot be empty.", "Invalid Input");
            return;
        }

        appointments.emplace_back(aid, pid, did, date, time);
        saveAppointmentsToFile(owner);
        GuiShowMessage(owner, "Appointment Booked Successfully!", "Success");
    }

    void viewAppointments(HWND owner = NULL) {
        if (appointments.empty()) {
            GuiShowMessage(owner, "No Appointments Found!", "Appointments");
            return;
        }

        ostringstream ss;
        for (const auto& a : appointments) {
            ss << "===================================\n";
            ss << "APPOINTMENT DETAILS\n";
            ss << "===================================\n";
            ss << "Appointment ID : " << a.getAppointmentID() << "\n";
            ss << "Patient ID     : " << a.getPatientID() << "\n";
            ss << "Doctor ID      : " << a.getDoctorID() << "\n";
            ss << "Date           : " << a.getDate() << "\n";
            ss << "Time           : " << a.getTime() << "\n\n";
        }
        GuiShowMessage(owner, ss.str(), "Appointments");
    }

    void savePatientsToFile(HWND owner = NULL) {
        ofstream file("patients.txt");
        for (const auto& p : patients) {
            file << p.getID() << ","
                 << p.getName() << ","
                 << p.getAge() << ","
                 << p.getGender() << ","
                 << p.getDisease() << ","
                 << p.getBloodGroup() << "\n";
        }
        file.close();
        GuiShowMessage(owner, "Patient Data Saved Successfully!", "Saved");
    }

    void saveDoctorsToFile(HWND owner = NULL) {
        ofstream file("doctors.txt");
        for (const auto& d : doctors) {
            file << d.getID() << ","
                 << d.getName() << ","
                 << d.getAge() << ","
                 << d.getGender() << ","
                 << d.getSpecialization() << ","
                 << d.getExperience() << "\n";
        }
        file.close();
        GuiShowMessage(owner, "Doctor Data Saved Successfully!", "Saved");
    }

    void saveAppointmentsToFile(HWND owner = NULL) {
        ofstream file("appointments.txt");
        for (const auto& a : appointments) {
            file << a.getAppointmentID() << ","
                 << a.getPatientID() << ","
                 << a.getDoctorID() << ","
                 << a.getDate() << ","
                 << a.getTime() << "\n";
        }
        file.close();
        GuiShowMessage(owner, "Appointment Data Saved Successfully!", "Saved");
    }
};

Hospital* gHospital = nullptr;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_COMMAND) {
        int id = LOWORD(wParam);
        switch (id) {
            case 2001: gHospital->addPatient(hwnd); break;
            case 2002: gHospital->viewPatients(hwnd); break;
            case 2003: gHospital->searchPatient(hwnd); break;
            case 2004: gHospital->deletePatient(hwnd); break;
            case 2005: gHospital->addDoctor(hwnd); break;
            case 2006: gHospital->viewDoctors(hwnd); break;
            case 2007: gHospital->searchDoctor(hwnd); break;
            case 2008: gHospital->bookAppointment(hwnd); break;
            case 2009: gHospital->viewAppointments(hwnd); break;
        }
    }
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // Show login dialog first
    if (!GuiShowLoginDialog("Hospital Management System - Login")) {
        return 0;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char CLASS_NAME[] = "HMS_Main_Window";

    WNDCLASSA wc{};
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassA(&wc)) {
        cout << "Failed to register main window class." << endl;
        return 0;
    }

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Hospital Management System",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, 460, 600,
                                NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        cout << "Failed to create main window." << endl;
        return 0;
    }

    gHospital = new Hospital();

    const int buttonWidth = 200;
    const int buttonHeight = 40;
    const int x1 = 20;
    const int x2 = 230;
    const int yStart = 20;
    const int yGap = 55;

    CreateWindowExA(0, "BUTTON", "Add Patient", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x1, yStart, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2001), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "View Patients", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x2, yStart, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2002), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "Search Patient", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x1, yStart + yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2003), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "Delete Patient", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x2, yStart + yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2004), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "Add Doctor", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x1, yStart + 2 * yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2005), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "View Doctors", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x2, yStart + 2 * yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2006), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "Search Doctor", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x1, yStart + 3 * yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2007), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "Book Appointment", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x2, yStart + 3 * yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2008), hInstance, NULL);
    CreateWindowExA(0, "BUTTON", "View Appointments", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x1, yStart + 4 * yGap, buttonWidth, buttonHeight, hwnd, reinterpret_cast<HMENU>(2009), hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete gHospital;
    return 0;
}
