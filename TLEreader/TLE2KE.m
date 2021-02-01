%% MATLAB code for TLE propagation
close all; clearvars; clc;

%% Import file data
TLEname = "TLE.txt";
delimiterIn = ' '; % uses space as delimiter
skiplines = 2; % assumes one header line before TLE data, all Kepler Elements on seconds line
TLEdata = textscan(fopen(TLEname),'%s',1,'delimiter','\n', 'headerlines',skiplines); % gets string line
TLEdata = str2double(split(TLEdata{1,1},{'   ','  ',' '}));  % split on whitespace and convert to double
%% Extract Keplerian Elements
inc = TLEdata(3); %deg, inclination
raan = TLEdata(4); %deg, right ascension of ascending node
ecc = TLEdata(5) * 1e-7 ; %eccentricity, decimal point assumed at start, 7 chars wide
arg_pgee = TLEdata(6); %deg, argument of perigee
M = TLEdata(7); %deg, mean anomaly
n = TLEdata(8)*2*pi/86400; %rad/s from rev/day, mean motion

%% Convert Keplerian Elements to R,V
nu = nufromE(EfromM(M,ecc,1E-8), ecc); % get true anomaly from mean anomaly
nu = rad2deg(nu); %convert to degrees
a = (3.986004418e5 / n^2 ) ^(1/3); %km, semi-major axis from a = cuberoot(mu_grav/n^2)

[reci, veci] = kep2ECI(a, ecc, inc, raan, arg_pgee, nu); % reci in km, veci in km/s

%% Write data out
outfile = "KERV.txt";
outid = fopen(outfile,'w');
fprintf(outid,"Semi-major Axis (km), Eccentricity, Inclination (deg), RAAN (deg), Arg of Perigee (deg), Mean Anomaly (deg)\n");
fprintf(outid,"%f,%f,%f,%f,%f,%f\n",a,ecc,inc,raan,arg_pgee,M);
fprintf(outid,"R in ECI (km)\n");
fprintf(outid,"%f,%f,%f\n",reci);
fprintf(outid,"V in ECI (km/s)\n");
fprintf(outid,"%f,%f,%f\n",veci);

%% Helper Functions
function E = EfromM(M,e,ep)
    if e>= 0.5
        E = pi;
    else
        E = M;
    end
    d = delta(E,e,M);
    n = 0;
    while abs(d) > ep
       E = E+d;
       d = delta(E,e,M);
       n = n + 1;
       if n == 20
           break;
       end
    end
    
    function delta = delta(Ei,e,M)
        delta = (Ei - e*sin(Ei) - M)/(e*cos(Ei) - 1);
    end
end
function nu = nufromE(E,e)
    E = mod(E,2*pi);
    nu = acos((cos(E)-e)/(1-e*cos(E)));
    if ((E>=0 && E<=pi) && ~(nu>=0 && nu<=pi)) || ((E>pi && E<2*pi) && ~(nu>pi && nu<2*pi))
        nu = 2*pi - nu;
    end
end
function [r_eci, v_eci] = kep2ECI(a, e, i, raan, w, nu)
    if e == 0
       w = 0; 
    end
    if i == 0
       raan = 0; 
    end
    %Calculate additional useful orbit parameters
    mu_g = 3.986004418e5; %km^3/s^2
    p = a*(1-e^2);
    r = p/(1+e*cos(nu));
    v = sqrt((2*mu_g/r) - (mu_g/a));
    h = sqrt(mu_g * p);
    
    %Calculate angle between r and v vector
    gamma = asin(h/(r*v));
    %Calculate position and velocity vectors
    rotm = rotz(raan)*rotx(i)*rotz(w);
        % ... Rotation matrix from PQW to IJK 
    r_vec_kep = [r*cosd(nu); r*sind(nu); 0]; % PQW coordinates
    v_vec_kep = (1/r^2).*((v*r*cos(gamma)).*r_vec_kep - cross(r_vec_kep, [0;0;h])); %calculate v in PQW from h,r,norm(v) and gamma
    r_eci = (rotm*(r_vec_kep)); %multiply by rotm to go from PQW to IJK
    v_eci = (rotm*(v_vec_kep));
end