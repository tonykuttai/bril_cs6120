#include <iostream>
#include <fstream>

class DualOutputStream {
public:
    DualOutputStream(const std::string& fileName) {
        // Open the file for writing
        fileStream.open(fileName);
    }

    template <typename T>
    DualOutputStream& operator<<(const T& data) {
        // Write to the file
        fileStream << data;

        // Write to std::cout
        std::cout << data;

        return *this;
    }

    // Specialization for manipulators (e.g., std::endl)
    DualOutputStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        // Apply the manipulator to both streams
        manip(fileStream);
        manip(std::cout);
        return *this;
    }

    ~DualOutputStream() {
        // Close the file
        fileStream.close();
    }

private:
    std::ofstream fileStream;
};
