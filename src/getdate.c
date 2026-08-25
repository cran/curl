#include <curl/curl.h>
#include <Rinternals.h>

SEXP R_curl_getdate(SEXP datestring) {
  if(!Rf_isString(datestring))
    Rf_error("Argument 'datestring' must be string.");

  int len = Rf_length(datestring);
  SEXP out = PROTECT(Rf_allocVector(REALSXP, len));

  for(int i = 0; i < len; i++){
    time_t date = curl_getdate(CHAR(STRING_ELT(datestring, i)), NULL);
    REAL(out)[i] = date < 0 ? NA_REAL : (double) date;
  }
  UNPROTECT(1);
  return out;
}
