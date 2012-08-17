A=dir('*.wav')
for i = 1:length(A)
  n=A(i).name;
  [Y,fs,bits] = wavread(n);
  n2=sprintf("%spi",n(1:end-3));
  fid=fopen(n2,"wb");
  fwrite(fid,(Y*32678),"int16");
  fclose(fid);
end