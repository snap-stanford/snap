double TStopwatch::Tick() {
  //return clock() / ((double)CLOCKS_PER_SEC);
  #ifdef _OPENMP
  return omp_get_wtime();
  #else
  #ifdef GLib_WIN32
  return GetTickCount() / 1000.0;
  #else
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);

  float cputime =
  ((float) (rusage.ru_utime.tv_usec + rusage.ru_stime.tv_usec) / 1000000) +
  ((float) (rusage.ru_utime.tv_sec + rusage.ru_stime.tv_sec));
  return cputime;
  #endif
  #endif
}

void TStopwatch::Start(const TExperiment Exp) {
  Starts[Exp] = Tick();
}

void TStopwatch::Stop(const TExperiment Exp) {
  double Duration = Tick() - Starts[Exp];
  Sums[Exp] += Duration;
  Maxs[Exp] = Maxs[Exp] >= Duration ? Maxs[Exp] : Duration;
  Mins[Exp] = Mins[Exp] <= Duration ? Mins[Exp] : Duration;
  Cnts[Exp]++;
}

int TStopwatch::Cnt(const TExperiment Exp) const {
  return Cnts[Exp];
}

double TStopwatch::Sum(const TExperiment Exp) const {
  return Sums[Exp];
}

double TStopwatch::Avg(const TExperiment Exp) const {
  return Sums[Exp] / Cnts[Exp];
}

double TStopwatch::Max(const TExperiment Exp) const {
  return Maxs[Exp];
}

double TStopwatch::Min(const TExperiment Exp) const {
  return Mins[Exp];
}
