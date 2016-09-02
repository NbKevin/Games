/**
 * This file contains the source code of homework #1 of course CS-222 of
 * NYU Shanghai. Its author Bin Ni, bn628, kevin.nb#nyu.edu.
 *
 * Homework #1 focuses on a fully operational life game.
 *
 * Tested under g++ 6.2.0, POSIX thread model, C++11, MinGW64.
 * Tested under clang++ 3.5.0, POSIX thread model, C++11.
 * Tested under valgrind 3.1.0.
 */


#include <stdlib.h>
#include <iomanip>
#include <sstream>
// clang++/g++ requires explicit inclusion of stdexcept
// maybe it's mingw64 who automatically included it?
#include <stdexcept>
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;


/**
 * Macros for more readable code and constants.
 * Supposingly they have not been defined so no redefinition/namespace
 * trick is done here.
 */
#define DEFAULT_GENERATION 10
#define DEFAULT_COLUMNS 20
#define DEFAULT_ROWS 10
#define DEFAULT_FILE_NAME "life.txt"
#define DEFAULT_ALIVE_REPRESENTATION "*"
#define DEFAULT_NOT_ALIVE_REPRESENTATION "-"
#define DEFAULT_DELIMITER " "

/**
 * Automated debug configuration.
 */
#if !defined(NDEBUG)  // for cmake
#define __HW1__DEBUG
#elif defined(DEBUG)  // for apple/llvm
#define __HW1__DEBUG
#elif defined(_DEBUG)  // for vs/cl
#define __HW1__DEBUG
#endif

/**
 * Portable definition of unsigned and signed intergrals.
 */
typedef size_t _unsigned;
typedef ptrdiff_t _signed;

/**
 * Grid is represented as a two-dimensional vector.
 */
typedef vector<vector<bool> > _grid_t;

/**
 * Wrapped interface for grid operations.
 * Vector operation is done through std::vector::at over std::vector::operator[] to take
 * advantage of its range checking to avoid unexpected behaviour in
 * case of out of boundary exception.
 */
class Grid {
public:
    /**
     * Creates a grid with given number of rows and columns.
     * The grid contains first row and then column.
     * @param __column Number of columns.
     * @param __row Number of rows.
     * @param __default_val Value to fill in the grid.
     */
    Grid(bool __default_val = false,
         _unsigned __row = DEFAULT_ROWS, _unsigned __column = DEFAULT_COLUMNS) :
            grid(__row, vector<bool>(__column, __default_val)) {
    }

    /**
     * Construct from another grid.
     * @param __src Source grid.
     */
    Grid(const Grid &__src) {
        this->grid = __src.grid;
    }

    /**
     * Assign this instance the content of another grid.
     * @param __src Source grid.
     * @return The assigned instance.
     */
    Grid &operator=(const Grid &__src) {
        this->grid = __src.grid;
        return *this;
    }

    /**
     * Inspect whether a coordinate is within the boundary of the given grid.
     * Grid containing no row or no column would invalidate all coordinate.
     * @param __row The row.
     * @param __column The column.
     * @return Whether the coordinate is valid.
     */
    bool inspect_coord(_signed __row, _signed __column) const {
        _unsigned rows = this->rows();
        _unsigned columns = this->columns();
        if (rows <= 0 || columns <= 0) return false;
        if (__row < 0 || __column < 0) return false;
        return rows > __row && columns > __column;
    }

    /**
     * Inspect the cell to see whether it is still alive.
     * Cell of invalid coordinate would not be alive.
     * @param __row The row the cell is in.
     * @param __column The column the cell is in.
     * @return If it is alive, false if row of column does not exist.
     */
    bool inspect_cell(_signed __row, _signed __column) const {
        if (!this->inspect_coord(__row, __column)) return false;
        return this->grid.at((_unsigned) __row).at((_unsigned) __column);
    }

    /**
     * Examine whether a cell would be alive or not after next iteration.
     * @param __row The row the cell is in.
     * @param __column The column the cell is in.
     * @return If it would remain alive.
     */
    bool predict_cell(_signed __row, _signed __column) const {
        if (!this->inspect_coord(__row, __column)) return false;

        _unsigned neighbours = 0;
        for (_signed row_offset = -1; row_offset <= 1; row_offset++) {
            for (_signed column_offset = -1; column_offset <= 1; column_offset++) {
                if (row_offset == 0 && column_offset == 0)
                    continue;

                _signed row_in_focus = __row + row_offset;
                _signed column_in_focus = __column + column_offset;
                if (this->inspect_cell(row_in_focus, column_in_focus))
                    neighbours++;
            }
        }

#ifdef __HW1__DEBUG
        cout << "HW1_DEBUG: Cell at ("
             << setfill(' ') << setw(2) << __row << ", "
             << setfill(' ') << setw(2) << __column << ") neighbours "
             << neighbours << endl;
#endif
        if (neighbours == 3) return true;
        if (neighbours != 2) return false;
        return this->inspect_cell(__row, __column);
    }

    /**
     * Iterate one generation in place.
     */
    void iterate() {
        auto temp_grid = *this;
        for (_unsigned row = 0; row < this->rows(); row++) {
            for (_unsigned column = 0; column < this->columns(); column++) {
#if defined(__HW1__DEBUG)
                auto prediction = this->predict_cell(row, column);
                cout << "HW1_DEBUG: Cell at ("
                     << setfill(' ') << setw(2) << row << ", "
                     << setfill(' ') << setw(2) << column << ") predicts "
                     << prediction << endl;
                temp_grid.grid.at(row).at(column) = prediction;
#else
                temp_grid.grid.at(row).at(column) = this->predict_cell(row, column);
#endif
            }
        }
        *this = temp_grid;
    }

    /**
     * Iterate given times of generation, in place.
     * @param __times Times to iterate.
     */
    void iterate(_unsigned __times) {
        for (_unsigned time = 0; time < __times; time++)
            this->iterate();
    }

    /**
     * Find the number of rows in the grid.
     * @return The number of rows.
     */
    inline _unsigned rows() const {
        return this->grid.size();
    }

    /**
     * Find the number of columns in the grid.
     * @return The number of columns.
     */
    inline _unsigned columns() const {
        if (this->rows() == 0) return 0;
        return this->grid[0].size();
    }

    /**
     * Display the grid.
     */
    void display(const char *__alive_representation = DEFAULT_ALIVE_REPRESENTATION,
                 const char *__not_alive_representation = DEFAULT_NOT_ALIVE_REPRESENTATION,
                 const char *__delimiter = DEFAULT_DELIMITER) const {
        for (const auto &row: this->grid) {
            for (const auto &cell: row) {
                cout << (cell ? __alive_representation : __not_alive_representation);
                cout << __delimiter;
            }
            cout << endl;
        }
        cout << endl;
    }

    /**
     * Parse from a file a grid and assign it to this grid.
     * @param __file_name File name.
     */
    void parse_from_file(const char *__file_name,
                         const char __alive_representation = *DEFAULT_ALIVE_REPRESENTATION) {
        ifstream file(__file_name);
        if (!file)
            throw runtime_error("Error opening file, probably bad file name or insufficient privilege?");

        _unsigned rows = this->rows();
        _unsigned columns = this->columns();
        _unsigned row = 0;
        _unsigned column = 0;
        char ch;

        while (file >> noskipws >> ch) {
            if (ch == '\r' || ch == '\n') {  // carriage return consisting of \r\n is not supported yet
                while (column < columns)  // make up missing columns
                    this->grid.at(row).at(column++) = false;
                if (++row >= rows) break;
                column = 0;
            } else {
                if (column >= columns) continue;  // ignore exceeding columns
                this->grid.at(row).at(column++) = ch == __alive_representation;
            }
        }

        file.close();
    }

private:
    _grid_t grid;
};


/**
 * Main entry point.
 * @return Result code.
 */
int main(int argc, char *argv[]) {
    try {
#if defined(__HW1__DEBUG)
        cout << "HW1_DEBUG: Running using DEBUG configuration" << endl;
#endif

        const _signed generation = argc >= 2 ? stoll(argv[1]) : DEFAULT_GENERATION;
        const char *file_name = argc >= 3 ? argv[2] : DEFAULT_FILE_NAME;
        const _signed rows = argc >= 4 ? stoll(argv[3]) : DEFAULT_ROWS;
        const _signed columns = argc >= 5 ? stoll(argv[4]) : DEFAULT_COLUMNS;

        if (generation < 0 || rows < 0 || columns < 0)
            throw runtime_error("[generation] [rows] and [columns] must all be positive");

#if defined(__HW1__DEBUG)
        cout << "HW1_DEBUG: Read " << argc - 1 << " argument(s)" << endl;
        cout << "HW1_DEBUG: Using generation <" << generation << ">" << endl;
        cout << "HW1_DEBUG: Using file name <" << file_name << ">" << endl;
        cout << "HW1_DEBUG: Using rows <" << rows << ">" << endl;
        cout << "HW1_DEBUG: Using columns <" << columns << ">" << endl;
#endif

        auto grid = Grid(false, (_unsigned) rows, (_unsigned) columns);
        grid.parse_from_file(file_name);
        cout << "Grid read from file (generation 0): " << endl;
        grid.display();
        for (_unsigned gen_index = 1; gen_index <= generation; gen_index++) {
            grid.iterate();
            cout << "Generation " << gen_index << ": " << endl;
            grid.display();
        }
    } catch (const exception &e) {
        cerr << "Program failed: " << e.what() << endl;
        return -1;
    }
}
