#include <iostream>
#include <stdio.h>
#define WIN32
#include "lp_lib.h"

using namespace std;

main()
{
  lprec *lp;
  HINSTANCE lpsolve;
  make_lp_func *_make_lp;
  delete_lp_func *_delete_lp;
  read_LP_func *_read_LP;
  get_Nrows_func *_get_Nrows;
  write_lp_func *_write_lp;
  solve_func *_solve;
  get_objective_func *_get_objective;
  get_Ncolumns_func *_get_Ncolumns;
  get_constraints_func *_get_constraints;

  lpsolve = LoadLibrary("lpsolve55.dll");

  if (lpsolve == NULL) {
    printf("Unable to load lpsolve shared library\n");
    return(FALSE);
  }
  _delete_lp = (delete_lp_func *) GetProcAddress(lpsolve, "delete_lp");
  _read_LP = (read_LP_func *) GetProcAddress(lpsolve, "read_LP");
  _get_Nrows = (get_Nrows_func *) GetProcAddress(lpsolve, "get_Nrows");
  _write_lp = (write_lp_func *) GetProcAddress(lpsolve, "write_lp");
  _solve = (solve_func *) GetProcAddress(lpsolve, "solve");
  _get_objective = (get_objective_func *) GetProcAddress(lpsolve, "get_objective");
  _get_Ncolumns = (get_Ncolumns_func *) GetProcAddress(lpsolve, "get_Ncolumns");
  _get_constraints = (get_constraints_func *) GetProcAddress(lpsolve, "get_constraints");


  char a[100] = "input.txt";
  char b[10] = "model.lp";
  double ans[20];
  lp = _read_LP(a, 1, a);
    _solve(lp);
    cout << _get_Nrows(lp) << "   " << _get_Ncolumns(lp) << endl;
    double obj = _get_objective(lp);
    cout << obj << endl;
    //_write_lp(lp, b);
  /* ... */

  _delete_lp(lp);

  FreeLibrary(lpsolve);
}
