clc; close all; clear all;

fs = 48000;
T = 1/fs;
A = 0.5;
f = 4000;
DCoff = 1.5;
N = 480;
t = single(0:N-1)*T;

x = DCoff + A*sin(2*pi*f*t); 
x = floor(x*255/3.3); x = single(x');

% fprintf("{");
% for i = 1:N
%    fprintf("%d, ", x(i));
%    if(mod(i, 100) == 0)
%        fprintf("\n");
%    end
% end
% fprintf("}");

m = N;
n = 1;
k = 545;
f1 = 42/512; f2 = 44/512;
a = single(exp(1j*2*pi*f1));
w = single(exp(-1j*2*pi*(f2-f1)/k));

%------- Length for power-of-two fft.
nfft = 2^nextpow2(m+k-1);
%------- Premultiply data.
kk = single( (-m+1):max(k-1,m-1) ).';
kk2 = (kk .^ 2) ./ 2;
ww = w .^ (kk2);    % <----- Chirp filter is 1./ww
nn = single(0:(m-1))';
aa = a .^ ( -nn );
aa = aa.*ww(m+nn);
y = x .* aa(:,ones(1,n));
%------- Fast convolution via FFT.
fy = fft( y, nfft );
fv = fft( 1 ./ ww(1:(k-1+m)), nfft );    % <----- Chirp filter.
fy = fy .* fv(:,ones(1, n));
g = ifft(fy);
%------- Final multiply.
g = g( m:(m+k-1), : ) .* ww(m:(m+k-1),ones(1, n));

g = abs(g);
[~, maxidx] = max(g); 
figure, plot(g)
fprintf("GZT maxidx, f: %d, %f\n", maxidx-1, f1*fs+(maxidx-1)*(f2-f1)*fs/k);

