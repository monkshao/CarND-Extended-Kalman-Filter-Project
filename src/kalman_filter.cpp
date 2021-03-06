#include "kalman_filter.h"
#include <iostream>
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

double normalizeAngle(double theta);


void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
//   long x_size = x_.size();
//   MatrixXd I = MatrixXd::Identity(x_size, x_size);
  MatrixXd I(4, 4);
  I << 1.0, 0, 0, 0,
       0, 1.0, 0, 0,
       0, 0, 1.0, 0,
       0, 0, 0, 1.0;
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  //The radar sensor will output values in polar coordinates
  //In order to calculate yy for the radar sensor, we need to convert x' to polar coordinates. 
  // calculate x object state to polar coordinates
  double dot = x_(0)*x_(0) + x_(1)*x_(1);
  double rho = sqrt(dot);
  double theta = atan2(x_(1), x_(0));
  double rho_dot;
  if (fabs(dot) < 0.001) { 
    rho_dot = 0;
    cout << "Calculate rho () - Error - Division by Zero; stop doing UpdateEKF" << endl;
    return;
  }
  else rho_dot = (x_(0)*x_(2) + x_(1)*x_(3)) / rho;
  VectorXd h_prime = VectorXd(3);
  h_prime << rho, theta, rho_dot;
  VectorXd y = z - h_prime;
  
  y[1] = normalizeAngle(y[1]);
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
//   long x_size = x_.size();
//   MatrixXd I = MatrixXd::Identity(x_size, x_size);
  MatrixXd I(4, 4);
  I << 1.0, 0, 0, 0,
       0, 1.0, 0, 0,
       0, 0, 1.0, 0,
       0, 0, 0, 1.0;
  P_ = (I - K * H_) * P_;
}

double normalizeAngle(double theta)
{
  // normalize the angle between -pi to pi
  while(theta > M_PI){
    theta -= 2 * M_PI;
  }

  while(theta < -M_PI){
    theta += 2 * M_PI;
  }
  return theta;
}

