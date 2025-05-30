test_that("File writer is properly cleaned", {
  fp1 <- file_writer(tempfile())
  fp2 <- file_writer(tempfile())
  expect_equal(total_writers(), 0)
  fp1(raw(5))
  fp1(raw(10))
  expect_equal(total_writers(), 1)
  fp2(raw(5))
  fp2(raw(10))
  gc()
  expect_equal(total_writers(), 2)
  fp2(raw(0), close = TRUE)
  expect_equal(total_writers(), 1)
  rm(fp1); gc()
  expect_equal(total_writers(), 0)
})

test_that("File writer is closed at the end of the data callback", {
  pool <- new_pool()
  tmp <- tempfile()
  on.exit(unlink(tmp))
  curl_fetch_multi(httpbin("get"), data = tmp, pool = pool)
  expect_equal(total_writers(), 0)
  multi_run(pool = pool)
  expect_equal(total_writers(), 0)
  json <- readLines(tmp)
  expect_true(jsonlite::validate(json))
})
