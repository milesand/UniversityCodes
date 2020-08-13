FROM nginx:stable
RUN rm /etc/nginx/conf.d/default.conf

COPY wiki.conf /etc/nginx/conf.d/
COPY wiki /var/www/wiki
