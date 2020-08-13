FROM php:7.2-fpm

RUN docker-php-ext-install mysqli
COPY wiki /var/www/wiki
RUN chown -R www-data:www-data /var/www
