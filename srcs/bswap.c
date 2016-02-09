unsigned long	my_bswap(unsigned long n)
{
  __asm__ ("bswap %0\n" : "=r" (n) : "r" (n) : "%rdi");
  return (n);
}
